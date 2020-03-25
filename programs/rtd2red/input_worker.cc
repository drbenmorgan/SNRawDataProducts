// Ourselves:
#include "input_worker.h"

namespace snfee {
  namespace redb {

    void
    input_worker::stop()
    {
      _stop_request_ = true;
      return;
    }

    bool
    input_worker::is_stopped() const
    {
      return _stop_request_;
    }

    input_worker::~input_worker()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      if (_preader_) {
        DT_LOG_DEBUG(_logging_, "Terminating the embedded reader...");
        _preader_.reset();
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    input_worker::input_worker(
      const int id_,
      std::mutex& imtx_,
      snfee::redb::rtd_buffer& ibuf_,
      const snfee::redb::builder_config::input_config_type& iconfig_,
      const datatools::logger::priority logging_)
      : _mtx_(imtx_), _buf_(ibuf_)
    {
      _logging_ = logging_;
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_LOG_DEBUG(_logging_, "Worker #" << id_);
      _id_ = id_;
      DT_LOG_DEBUG(_logging_,
                   "Open a multi-file reader from worker [" << _id_ << "]...");

      snfee::io::multifile_data_reader::config_type reader_config;
      if (!iconfig_.listname.empty()) {
        // Read a file containing a list of input filenames:
        std::string listname = iconfig_.listname;
        datatools::fetch_path_with_env(listname);
        std::ifstream finput_list(listname);
        DT_THROW_IF(!finput_list,
                    std::logic_error,
                    "Cannot open input files list file '" << listname << "'!");
        while (finput_list and !finput_list.eof()) {
          std::string line;
          std::getline(finput_list, line);
          boost::trim_copy(line);
          if (line.empty()) {
            continue;
          }
          std::istringstream ins(line);
          std::string filename;
          ins >> filename >> std::ws;
          if (!filename.empty() and filename[0] != '#') {
            reader_config.filenames.push_back(filename);
          }
        }
      }
      // Add any explicit filenames:
      for (int ifile = 0; ifile < (int)iconfig_.filenames.size(); ifile++) {
        reader_config.filenames.push_back(iconfig_.filenames[ifile]);
      }
      _preader_.reset(new snfee::io::multifile_data_reader(reader_config));
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void
    input_worker::print(std::ostream& out_) const
    {
      std::ostringstream out;
      out << "Input worker [@" << this << "] :" << std::endl;
      out << "|-- ID     : " << _id_ << std::endl;
      out << "|-- Mutex  : [@" << &_mtx_ << ']' << std::endl;
      out << "|-- Buffer : [@" << &_buf_ << ']' << std::endl;
      out << "|-- Reader : [@" << _preader_.get() << ']' << std::endl;
      out << "`-- Stop   : " << std::boolalpha << _stop_request_ << std::endl;
      out << std::endl;
      out_ << out.str();
      return;
    }

    std::size_t
    input_worker::get_records_counter() const
    {
      return _records_counter_;
    }

    void
    input_worker::run()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      _records_counter_ = 0;
      if (datatools::logger::is_debug(_logging_)) {
        DT_LOG_DEBUG(_logging_, "_preader_ = [@" << _preader_ << "]");
      }
      // Working RTD record:
      snfee::io::rtd_record rec;
      DT_LOG_DEBUG(_logging_,
                   "Starting main running loop from worker [" << _id_
                                                              << "]...");
      bool terminated_input = false;
      while (!_stop_request_) {
        DT_LOG_DEBUG(_logging_, "Do loop from worker [" << _id_ << "]...");
        if (rec.empty()) {
          // Try to load a new RTD record:
          if (_preader_->has_record_tag()) {
            if (_preader_->record_tag_is(
                  snfee::data::raw_trigger_data::SERIAL_TAG)) {
              DT_LOG_DEBUG(_logging_,
                           "Loading a new raw trigger data record from worker ["
                             << _id_ << "]...");
              rec.make_record();
              _preader_->load(*rec.get_rtd_ptr());
              DT_LOG_DEBUG(_logging_,
                           "New raw trigger data record is loaded from worker ["
                             << _id_ << "]...");
              _records_counter_++;
              DT_LOG_DEBUG(_logging_,
                           "Input buffer #" << _id_
                                            << " is unlocked by input worker.");
            } else {
              DT_THROW(std::logic_error,
                       "Worker [" << _id_ << "] met unknown serialized object '"
                                  << _preader_->get_record_tag() << "'");
            }
          } else {
            terminated_input = true;
          }
        }

        if (!rec.empty()) {
          std::lock_guard<std::mutex> lock(_mtx_);
          DT_LOG_DEBUG(_logging_,
                       "Input buffer #" << _id_
                                        << " is locked by input worker.");
          if (_buf_.can_push()) {
            // DT_LOG_NOTICE(_logging_, "Input worker [" << _id_ << "] run : can
            // push!");
            DT_LOG_DEBUG(_logging_,
                         "Inserting the RTD record in the input buffer #"
                           << _id_ << "...");
            _buf_.insert_record(rec);
            DT_LOG_DEBUG(_logging_,
                         "RTD record was inserted in the input buffer #"
                           << _id_ << "...");
            if (datatools::logger::is_debug(_logging_)) {
              _buf_.print(std::cerr);
            }
            rec.reset();
          } else {
            // DT_LOG_NOTICE(_logging_, "Input worker [" << _id_ << "] run :
            // cannot push!");
          }
        }

        if (terminated_input and rec.empty()) {
          // No more records from the reader and no waiting current record:
          std::lock_guard<std::mutex> lock(_mtx_);
          DT_LOG_DEBUG(_logging_,
                       "Input buffer #" << _id_
                                        << " is locked by input worker.");
          DT_LOG_DEBUG(_logging_, "RTD [" << _id_ << "] source is done.");
          _buf_.terminate();
          DT_LOG_DEBUG(_logging_,
                       "Input buffer #" << _id_
                                        << " is unlocked by input worker.");
          stop();
          if (is_stopped()) {
            DT_LOG_NOTICE(_logging_,
                          "Input buffer [" << _id_ << "] at worker stop: ");
            _buf_.print(std::cerr);
          }
        }

        DT_LOG_DEBUG(_logging_, "Worker [" << _id_ << "] yields...");
        if (_records_counter_ % 1000 == 0 or is_stopped()) {
          DT_LOG_NOTICE(_logging_,
                        "Input worker [" << _id_
                                         << "] run : " << _records_counter_
                                         << " input records");
        }
        std::this_thread::yield();
      } // end of run loop
      DT_LOG_NOTICE(_logging_, "Input worker [" << _id_ << "] run is stopped.");
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

  } // namespace redb
} // namespace snfee
