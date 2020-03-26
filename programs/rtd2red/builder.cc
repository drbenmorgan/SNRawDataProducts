// Ourselves:
#include "builder.h"

// Standard Library:
#include <chrono>
#include <condition_variable>
#include <deque>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/data/calo_hit_record.h>
#include <snfee/data/raw_event_data.h>
#include <snfee/data/raw_trigger_data.h>
#include <snfee/data/tracker_hit_record.h>
#include <snfee/data/trigger_record.h>
#include <snfee/data/utils.h>
#include <snfee/io/multifile_data_reader.h>
#include <snfee/io/multifile_data_writer.h>

#include "builder_pimpl.h"
#include "input_worker.h"
#include "output_worker.h"
#include "red_buffer.h"
#include "rtd2red_builder.h"
#include "rtd_buffer.h"

namespace snfee {
  namespace redb {

    builder::builder() = default;

    builder::~builder() = default;

    datatools::logger::priority
    builder::get_logging() const
    {
      return _logging_;
    }

    void
    builder::set_logging(const datatools::logger::priority l_)
    {
      _logging_ = l_;
    }

    bool
    builder::is_initialized() const
    {
      return _initialized_;
    }

    void
    builder::set_config(const builder_config& cfg_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "RET builder is already initialized!");
      _config_ = cfg_;
    }

    const builder_config&
    builder::get_config() const
    {
      return _config_;
    }

    void
    builder::initialize()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "RED builder is already initialized!");

      DT_THROW_IF(_config_.input_config.filenames.empty() and
                    _config_.input_config.listname.empty(),
                  std::logic_error,
                  "Missing input config!");

      _at_init_();
      _initialized_ = true;
      DT_LOG_TRACE_EXITING(_logging_);
    }

    void
    builder::terminate()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(
        !is_initialized(), std::logic_error, "RED builder is not initialized!");
      _initialized_ = false;
      _at_terminate_();
      DT_LOG_TRACE_EXITING(_logging_);
    }

    void
    builder::run()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(
        !is_initialized(), std::logic_error, "RED builder is not initialized!");
      _at_run_();
      DT_LOG_TRACE_EXITING(_logging_);
    }

    bool
    builder::is_stopped() const
    {
      return _stop_request_;
    }

    void
    builder::stop()
    {
      _stop_request_ = true;
    }

    const std::vector<builder::worker_results_type>&
    builder::get_results() const
    {
      return _results_;
    }

    // virtual
    void
    builder::print_tree(std::ostream& out_,
                        const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      std::ostringstream outs;

      if (popts.title.length() != 0U) {
        outs << popts.indent << popts.title << std::endl;
      }

      outs << popts.indent << tag
           << "Logging : " << datatools::logger::get_priority_label(_logging_)
           << "'" << std::endl;

      outs << popts.indent << tag << "Configuration : " << std::endl;
      {
        boost::property_tree::ptree options;
        std::ostringstream iouts;
        iouts << popts.indent << skip_tag;
        options.put("indent", iouts.str());
        _config_.print_tree(outs, options);
      }

      outs << popts.indent << inherit_tag(popts.inherit)
           << "Initialized : " << std::boolalpha << is_initialized()
           << std::endl;

      out_ << outs.str();
    }

    // ============================ Private ============================= //

    void
    builder::_at_init_()
    {
      _pimpl_.reset(new builder_pimpl_type);
      builder_pimpl_type& pimpl = *_pimpl_;

      // Ouput manager:
      DT_LOG_NOTICE(_logging_, "Instantiating the output worker...");
      pimpl.omtx = std::make_shared<std::mutex>();
      pimpl.oworker = std::make_shared<output_worker>(
        *(pimpl.omtx), pimpl.obuffer, _config_.output_config, _logging_);

      // Builder:
      DT_LOG_NOTICE(_logging_, "Instantiating the builder...");
      pimpl.builder = std::make_shared<rtd2red_builder>(
        *_pimpl_, _config_.run_id, _config_.build_algo, _logging_);

      // Input manager:
      {
        DT_LOG_NOTICE(_logging_, "Instantiating the input buffer mutex...");
        pimpl.imtx = std::make_shared<std::mutex>();
        DT_LOG_NOTICE(_logging_, "Instantiating the input RTD buffer...");
        std::size_t min_popping_trig_ids = 1;
        uint32_t capacity = _config_.rtd_buffer_capacity;
        rtd_buffer buf(capacity, min_popping_trig_ids);
        pimpl.ibuffer = buf;
      }

      // Input buffer:
      {
        DT_LOG_NOTICE(_logging_, "Input buffer...");
        pimpl.ibuffer.print(std::cerr);
      }

      // Input worker:
      {
        DT_LOG_NOTICE(_logging_, "Instantiating the input worker...");
        pimpl.iworker = std::make_shared<input_worker>(0,
                                                       *(pimpl.imtx),
                                                       pimpl.ibuffer,
                                                       _config_.input_config,
                                                       _logging_);
        if (datatools::logger::is_debug(_logging_)) {
          pimpl.iworker->print(std::cerr);
        }
      }
    }

    void
    builder::_at_terminate_()
    {
      if (_pimpl_) {
        builder_pimpl_type& pimpl = *_pimpl_;

        if (pimpl.oworker) {
          worker_results_type owResults;
          owResults.category = WORKER_OUTPUT_RED;
          owResults.processed_records_counter1 =
            pimpl.oworker->get_records_counter();
          owResults.processed_records_counter2 =
            pimpl.oworker->get_stored_records_counter();
          _results_.push_back(owResults);
        }

        if (pimpl.iworker) {
          worker_results_type iwResults;
          iwResults.category = WORKER_INPUT_RTD;
          iwResults.processed_records_counter1 =
            pimpl.iworker->get_records_counter();
          _results_.push_back(iwResults);
          DT_LOG_DEBUG(_logging_, "Destroying the input worker...");
          pimpl.iworker.reset();
        }

        if (pimpl.builder) {
          // pimpl.merger->stop();
          DT_LOG_DEBUG(_logging_, "Destroying the builder...");
          pimpl.builder.reset();
        }

        if (pimpl.oworker) {
          // pimpl.oworker->stop();
          DT_LOG_DEBUG(_logging_, "Destroying the output worker...");
          pimpl.oworker.reset();
        }

        _pimpl_.reset();
      }
    }

    void
    builder::_at_run_()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      builder_pimpl_type& pimpl = *_pimpl_;

      // DT_LOG_DEBUG(_logging_, "Sleep a while first...");
      // std::this_thread::sleep_for(std::chrono::seconds(1));

      // Input worker threads:
      DT_LOG_DEBUG(_logging_, "Starting thread for input worker...");
      input_worker& iwrk = *pimpl.iworker;
      std::thread ithread(&input_worker::run, std::ref(iwrk));
      DT_LOG_DEBUG(_logging_, "Input worker has been launched.");

      // Builder thread:
      DT_LOG_DEBUG(_logging_, "Starting thread for builder...");
      rtd2red_builder& thebuilder = *pimpl.builder;
      std::thread bthread(&rtd2red_builder::run, std::ref(thebuilder));
      DT_LOG_DEBUG(_logging_, "Builder has been launched.");

      // Output worker thread:
      DT_LOG_DEBUG(_logging_, "Starting thread for output worker...");
      output_worker& owrk = *pimpl.oworker;
      std::thread othread(&output_worker::run, std::ref(owrk));
      DT_LOG_DEBUG(_logging_, "Output worker has been launched.");

      // Join threads:
      ithread.join();
      bthread.join();
      othread.join();

      DT_LOG_TRACE_EXITING(_logging_);
    }

  } // namespace redb
} // namespace snfee
