// Ourselves:
#include "output_worker.h"

namespace snfee {
  namespace redb {

    output_worker::output_worker(
      std::mutex& omtx_,
      red_buffer& obuf_,
      const snfee::redb::builder_config::output_config_type& oconfig_,
      const datatools::logger::priority logging_)
      : _mtx_(omtx_), _buf_(obuf_)
    {
      _logging_ = logging_;
      snfee::io::multifile_data_writer::config_type writer_config;
      writer_config.filenames = oconfig_.filenames;
      writer_config.max_records_per_file = oconfig_.max_records_per_file;
      writer_config.max_total_records = oconfig_.max_total_records;
      writer_config.terminate_on_overrun = oconfig_.terminate_on_overrun;
      _pwriter_ = std::make_shared<snfee::io::multifile_data_writer>(writer_config);
    }

    output_worker::~output_worker()
    {
      if (_pwriter_) {
        _pwriter_.reset();
      }
    }

    void
    output_worker::stop()
    {
      _stop_request_ = true;
    }

    bool
    output_worker::is_stopped() const
    {
      return _stop_request_;
    }

    std::size_t
    output_worker::get_records_counter() const
    {
      return _records_counter_;
    }

    std::size_t
    output_worker::get_stored_records_counter() const
    {
      return _stored_records_counter_;
    }

    void
    output_worker::run()
    {
      DT_LOG_TRACE_EXITING(_logging_);
      bool writer_is_terminated = false;
      _records_counter_ = 0;
      _stored_records_counter_ = 0;
      while (!_stop_request_) {
        {
          std::lock_guard<std::mutex> lock(_mtx_);
          while (!_buf_.is_empty()) {
            DT_LOG_DEBUG(_logging_,
                         "Pop RED record from the output RED buffer...");
            _records_counter_++;
            snfee::io::red_record rec = _buf_.pop_record();
            if (!writer_is_terminated and _pwriter_->is_terminated()) {
              writer_is_terminated = true;
              DT_LOG_NOTICE(_logging_, "Output RED writer is now terminated.");
            }
            if (!writer_is_terminated) {
              DT_LOG_DEBUG(_logging_, "Store the RED record.");
              _pwriter_->store(rec.get_red());
              _stored_records_counter_++;
              DT_LOG_DEBUG(_logging_, "RED record is stored.");
            } else {
              // Anticipated stop because writer is terminated:
              DT_LOG_NOTICE(_logging_, "Output RED writer: anticipated stop.");
              stop();
            }
          }
          if (_buf_.is_empty() && _buf_.is_terminated()) {
            DT_LOG_DEBUG(_logging_, "Output RED buffer is finished.");
            if (datatools::logger::is_debug(_logging_)) {
              _buf_.print(std::cerr);
            }
            DT_LOG_DEBUG(_logging_, "Request the output worker to stop.");
            stop();
          }
        }
        // DT_LOG_DEBUG(_logging_, "Processed records counter : " <<
        // _records_counter_); DT_LOG_DEBUG(_logging_, "Stored records counter
        // : " << _stored_records_counter_);
        if (_records_counter_ % 100 == 0 or is_stopped()) {
          DT_LOG_NOTICE(_logging_,
                        "Output worker run : " << _records_counter_
                                               << " saved RED records");
        }
        std::this_thread::yield();
      }
      DT_LOG_NOTICE(_logging_, "Output worker run is stopped.");
      DT_LOG_TRACE_EXITING(_logging_);
    }

  } // namespace redb
} // namespace snfee
