// Ourselves:
#include "builder.h"

// Standard Library:
#include <chrono>
#include <condition_variable>
#include <deque>
#include <fstream>
#include <iostream>
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
#include "rhd_record.h"
#include "rtd_record.h"
#include <snfee/data/calo_hit_record.h>
#include <snfee/data/raw_trigger_data.h>
#include <snfee/data/tracker_hit_record.h>
#include <snfee/data/trigger_record.h>
#include <snfee/data/utils.h>
#include <snfee/io/multifile_data_reader.h>
#include <snfee/io/multifile_data_writer.h>

namespace snfee {
  namespace rtdb {

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
                  "RDT builder is already initialized!");
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
                  "RDT builder is already initialized!");

      DT_THROW_IF(_config_.input_configs.empty(),
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
        !is_initialized(), std::logic_error, "RDT builder is not initialized!");
      _initialized_ = false;
      _at_terminate_();
      DT_LOG_TRACE_EXITING(_logging_);
    }

    void
    builder::run()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(
        !is_initialized(), std::logic_error, "RDT builder is not initialized!");
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

    // Forward declarations:
    struct rhd_buffer;
    struct rhd2rtd_merger;
    struct rtd_buffer;
    struct input_worker;
    struct output_worker;

    /// Smart pointer to a RHD input worker
    using input_worker_ptr = std::shared_ptr<input_worker>;

    /// Smart pointer to a RTD output worker
    using output_worker_ptr = std::shared_ptr<output_worker>;

    /// Smart pointer to a RHD merger
    using rhd2rtd_merger_ptr = std::shared_ptr<rhd2rtd_merger>;

    /// \brief RHD input buffer
    ///
    /// \code
    /// min_popping_trig_ids = 2
    /// current_trig_id      = 4
    /// rhd_recs             =
    /// [TRIGID=4][TRIGID=4][TRIGID=4][TRIGID=5][TRIGID=5][TRIGID=7][TRIGID=8][TRIGID=8]
    /// trig_ids             = {4,5,7,8}
    /// terminated           = false
    /// \endcode
    ///
    struct rhd_buffer {
      rhd_buffer(const int32_t id_,
                 const uint32_t capacity_ = 0,
                 const std::size_t min_popping_trig_ids_ = 2)
      {
        _id_ = id_;
        _min_popping_trig_ids_ = min_popping_trig_ids_;
        _capacity_ = capacity_;
      }

      ~rhd_buffer() = default;

      std::size_t
      size() const
      {
        return _rhd_recs_.size();
      }

      std::size_t
      get_capacity() const
      {
        return _capacity_;
      }

      /// Reset the RHD buffer
      void
      reset()
      {
        _rhd_recs_.clear();
        _front_trig_id_ = snfee::data::INVALID_TRIGGER_ID;
      }

      /// Check if the buffer is finished
      bool
      is_finished() const
      {
        return is_empty() and is_terminated();
      }

      int32_t
      get_front_trig_id() const
      {
        return _front_trig_id_;
      }

      bool
      can_push() const
      {
        return !(_capacity_ > 0 and _rhd_recs_.size() >= _capacity_);
      }

      /// Insert a new record in the buffer
      void
      insert_record(const snfee::io::rhd_record& rhd_rec_)
      {
        snfee::data::trigger_id_comparator_type trigger_id_less;
        int32_t new_trig_id = rhd_rec_.get_trigger_id();
        if (_rhd_recs_.empty()) {
          // RHD records =      [ ]
          // New record  = [6]---^
          _rhd_recs_.push_back(rhd_rec_);
          _front_trig_id_ = new_trig_id;
        } else {
          // Protect against unsorted input:
          if (trigger_id_less(new_trig_id, _front_trig_id_)) {
            // Unsorted case:
            // RHD records =      [
            // ][4][4][4][5][5][6][6][6][6][7][7][7][9][9][9][9][9] New record
            // = [3]---^
            DT_THROW(std::logic_error,
                     "Unsorted input RHD: new trigger ID="
                       << new_trig_id << "<"
                       << "front trigger ID=" << _front_trig_id_
                       << "! This is a bug!");
          }
          if (trigger_id_less(new_trig_id, *_trig_ids_.rbegin())) {
            // Unsorted case (rhd_rec_.get_trigger_id() < *_trig_ids_.rbegin() :
            // RHD records = [4][4][4][5][5][6][6][6][6][7][7][7][9][9][9][9][9]
            // New record  = [6]-------------^                    ^
            // New record  = [8]----------------------------------'
            DT_THROW(std::logic_error,
                     "Unsorted input RHD: new trigger ID="
                       << new_trig_id << "<"
                       << "last trigger ID=" << *_trig_ids_.rbegin()
                       << "! This is a bug!");
          }
          // Usual case:
          // RHD records = [4][4][4][5][5][6][6][ ]
          // New record  = [6]-------------------^
          // New record  = [8]-------------------'
          _rhd_recs_.push_back(rhd_rec_);
        }
        _trig_ids_.insert(new_trig_id);
      }

      /// Pop a record from the buffer
      snfee::io::rhd_record
      pop_record()
      {
        DT_THROW_IF(
          is_empty(), std::logic_error, "No more record in the RHD buffer!");
        snfee::io::rhd_record rec = _rhd_recs_.front();
        _rhd_recs_.pop_front();
        _front_trig_id_ = snfee::data::INVALID_TRIGGER_ID;
        _trig_ids_.clear();
        if (!_rhd_recs_.empty()) {
          // Update the current trigger ID with the ones from the next record:
          const snfee::io::rhd_record& next_rec = _rhd_recs_.front();
          _front_trig_id_ = next_rec.get_trigger_id();
          // Update the set of embedded trigger IDs:
          for (const auto& rhd_rec : _rhd_recs_) {
            _trig_ids_.insert(rhd_rec.get_trigger_id());
          }
        }
        return rec;
      }

      void
      terminate()
      {
        _terminated_ = true;
      }

      bool
      is_terminated() const
      {
        return _terminated_;
      }

      bool
      is_empty() const
      {
        return _rhd_recs_.empty();
      }

      int32_t
      get_next_poppable_trig_id() const
      {
        if (is_empty()) {
          return snfee::data::INVALID_TRIGGER_ID;
        }
        if (is_terminated()) {
          return _front_trig_id_;
        }
        if (_trig_ids_.size() >= _min_popping_trig_ids_) {
          return _front_trig_id_;
        }
        return snfee::data::INVALID_TRIGGER_ID;
      }

      bool
      can_be_popped() const
      {
        if (is_empty()) {
          return false;
        }
        if (is_terminated()) {
          return true;
        }
        if (_trig_ids_.size() >= _min_popping_trig_ids_) {
          return true;
        }
        return false;
      }

      void
      print(std::ostream& out_) const
      {
        std::ostringstream out;
        out << "RHD buffer : " << std::endl;
        out << "|-- ID : " << _id_ << std::endl;
        out << "|-- Minimum popping trig. IDs : " << _min_popping_trig_ids_
            << std::endl;
        out << "|-- Capacity : " << _capacity_ << std::endl;
        out << "|-- RHD records FIFO : " << _rhd_recs_.size() << std::endl;
        {
          std::size_t counter = 0;
          for (const auto& rhd_rec : _rhd_recs_) {
            out << "|   ";
            if (counter + 1 == _rhd_recs_.size()) {
              out << "`-- ";
            } else {
              out << "|-- ";
            }
            // out << "RHD";
            out << rhd_rec;
            out << std::endl;
            counter++;
          }
        }
        out << "|-- Front trigger ID : " << _front_trig_id_ << std::endl;
        out << "|-- Embedded trigger IDs : " << _trig_ids_.size() << std::endl;
        {
          std::size_t counter = 0;
          for (const auto& trigid : _trig_ids_) {
            bool print_it = false;
            if (counter == 0) {
              print_it = true;
            }
            if (counter + 1 == _trig_ids_.size()) {
              print_it = true;
            }
            if (print_it) {
              out << "|   ";
              if (counter + 1 == _trig_ids_.size()) {
                out << "`-- ";
              } else {
                out << "|-- ";
              }
              out << trigid;
              out << std::endl;
              if (counter == 0 and _trig_ids_.size() >= 3) {
                out << "|   "
                    << "|-- "
                    << "..." << std::endl;
              }
            }
            counter++;
          }
        }
        out << "|-- Terminated : " << std::boolalpha << _terminated_
            << std::endl;
        out << "`-- Finished : " << std::boolalpha << is_finished()
            << std::endl;
        out << std::endl;
        out_ << out.str();
      }

    private:
      // Configuration:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      int32_t _id_ = -1;       ///< FIFO identifier
      uint32_t _capacity_ = 0; ///< FIFO max capacity
      std::size_t _min_popping_trig_ids_ =
        1; ///< Minimal number of trigger IDs needed before popping
      std::deque<snfee::io::rhd_record> _rhd_recs_; ///< FIFO of RHD records
      std::set<int32_t> _trig_ids_; ///< Set of trigger IDs associated to
                                    ///< currently stored RHD records
      int32_t _front_trig_id_ =
        snfee::data::INVALID_TRIGGER_ID; ///< Front record trigger ID
      bool _terminated_ =
        false; ///< Input source is terminated and cannot push more RHD records
    };

    /// \brief RHD input worker
    struct input_worker {
      /// Request stop
      void
      stop()
      {
        _stop_request_ = true;
      }

      bool
      is_stopped() const
      {
        return _stop_request_;
      }

      /// Constructor
      input_worker(
        const int id_,
        std::mutex& imtx_,
        rhd_buffer& ibuf_,
        const snfee::rtdb::builder_config::input_config_type& iconfig_,
        const datatools::logger::priority logging_)
        : _mtx_(imtx_), _buf_(ibuf_)
      {
        _logging_ = logging_;
        DT_LOG_TRACE_ENTERING(_logging_);
        DT_LOG_DEBUG(_logging_, "Worker #" << id_);
        _id_ = id_;
        DT_LOG_DEBUG(_logging_,
                     "Open a multi-file reader from worker [" << _id_
                                                              << "]...");

        snfee::io::multifile_data_reader::config_type reader_config;
        if (!iconfig_.listname.empty()) {
          // Read a file containing a list of input filenames:
          std::string listname = iconfig_.listname;
          datatools::fetch_path_with_env(listname);
          std::ifstream finput_list(listname);
          DT_THROW_IF(!finput_list,
                      std::logic_error,
                      "Cannot open input files list filename!");
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
        for (const auto& filename : iconfig_.filenames) {
          reader_config.filenames.push_back(filename);
        }
        _preader_ =
          std::make_shared<snfee::io::multifile_data_reader>(reader_config);
        DT_LOG_TRACE_EXITING(_logging_);
      }

      /// Destructor
      ~input_worker()
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        if (_preader_) {
          DT_LOG_DEBUG(_logging_, "Terminating the embedded reader...");
          _preader_.reset();
        }
        DT_LOG_TRACE_EXITING(_logging_);
      }

      void
      run()
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        _records_counter_ = 0;
        if (datatools::logger::is_debug(_logging_)) {
          DT_LOG_DEBUG(_logging_, "_preader_ = [@" << _preader_ << "]");
        }
        // Working RHD record:
        snfee::io::rhd_record rec;
        DT_LOG_DEBUG(_logging_,
                     "Starting main running loop from worker [" << _id_
                                                                << "]...");
        bool terminated_input = false;
        while (!_stop_request_) {
          DT_LOG_DEBUG(_logging_, "Do loop from worker [" << _id_ << "]...");
          if (rec.empty()) {
            // Try to load a new RHD record:
            if (_preader_->has_record_tag()) {
              if (_preader_->record_tag_is(
                    snfee::data::calo_hit_record::SERIAL_TAG)) {
                DT_LOG_DEBUG(_logging_,
                             "Loading a new calo hit record from worker ["
                               << _id_ << "]...");
                rec.make_calo_hit();
                _preader_->load(*rec.get_calo_hit_rec());
                DT_LOG_DEBUG(_logging_,
                             "New calo hit record is loaded from worker ["
                               << _id_ << "]...");
                _records_counter_++;
                DT_LOG_DEBUG(_logging_,
                             "Input buffer #"
                               << _id_ << " is unlocked by input worker.");
              } else if (_preader_->record_tag_is(
                           snfee::data::tracker_hit_record::SERIAL_TAG)) {
                DT_LOG_DEBUG(_logging_,
                             "Loading a new tracker hit record from worker ["
                               << _id_ << "]...");
                rec.make_tracker_hit();
                _preader_->load(*rec.get_tracker_hit_rec());
                DT_LOG_DEBUG(_logging_,
                             "New tracker hit record is loaded from worker ["
                               << _id_ << "]...");
                _records_counter_++;
                DT_LOG_DEBUG(_logging_,
                             "Input buffer #"
                               << _id_ << " is unlocked by input worker.");
              } else if (_preader_->record_tag_is(
                           snfee::data::trigger_record::SERIAL_TAG)) {
                DT_LOG_DEBUG(_logging_,
                             "Loading a new trigger record from worker ["
                               << _id_ << "]...");
                rec.make_trig();
                _preader_->load(*rec.get_trig_rec());
                DT_LOG_DEBUG(_logging_,
                             "New trigger record is loaded from worker ["
                               << _id_ << "]...");
                _records_counter_++;
                DT_LOG_DEBUG(_logging_,
                             "Input buffer #"
                               << _id_ << " is unlocked by input worker.");
              } else {
                DT_THROW(std::logic_error,
                         "Worker [" << _id_
                                    << "] met unknown serialized object '"
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
              // DT_LOG_NOTICE(_logging_, "Input worker [" << _id_ << "] run :
              // can push!");
              DT_LOG_DEBUG(_logging_,
                           "Inserting the RHD record in the input buffer #"
                             << _id_ << "...");
              _buf_.insert_record(rec);
              DT_LOG_DEBUG(_logging_,
                           "RHD record was inserted in the input buffer #"
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
            DT_LOG_DEBUG(_logging_, "RHD [" << _id_ << "] source is done.");
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
        DT_LOG_NOTICE(_logging_,
                      "Input worker [" << _id_ << "] run is stopped.");
        DT_LOG_TRACE_EXITING(_logging_);
      }

      void
      print(std::ostream& out_) const
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
      }

      std::size_t
      get_records_counter() const
      {
        return _records_counter_;
      }

      // Configuration:
      datatools::logger::priority _logging_; ///< Logging priority
      int _id_ = -1;                         ///< Identifier of the input worker
      std::mutex&
        _mtx_; ///< Handle to the mutex associated to the input RHD buffer
      rhd_buffer& _buf_; ///< Handle to the input RHD buffer
      bool _accept_unsorted_input_ = false;
      std::shared_ptr<snfee::io::multifile_data_reader>
        _preader_; ///< Data reader

      // Working:
      bool _stop_request_ = false;       ///< Control stop
      std::size_t _records_counter_ = 0; ///< Counter of processed RHD records

    }; // end of struct input_worker

    /// \brief RTD output buffer
    struct rtd_buffer {

      rtd_buffer() = default;

      ~rtd_buffer()
      {
        _front_trigger_id_ = snfee::data::INVALID_TRIGGER_ID;
        _rtd_recs_.clear();
      }

      void
      terminate()
      {
        _terminated_ = true;
      }

      bool
      is_terminated()
      {
        return _terminated_;
      }

      bool
      is_empty()
      {
        return _rtd_recs_.empty();
      }

      void
      push_record(const snfee::io::rtd_record& rec_)
      {
        int32_t new_trigger_id = rec_.get_trigger_id();
        _front_trigger_id_ = new_trigger_id;
        _rtd_recs_.push_back(rec_);
      }

      snfee::io::rtd_record
      pop_record()
      {
        snfee::io::rtd_record rec = _rtd_recs_.front();
        _rtd_recs_.pop_front();
        _front_trigger_id_ = snfee::data::INVALID_TRIGGER_ID;
        if (!_rtd_recs_.empty()) {
          // Update the current trigger ID with the ones from the next record in
          // the FIFO:
          const snfee::io::rtd_record& next_rec = _rtd_recs_.front();
          _front_trigger_id_ = next_rec.get_trigger_id();
        }
        return rec;
      }

      void
      print(std::ostream& out_) const
      {
        std::ostringstream out;
        out << "RTD buffer: " << std::endl;
        out << "|-- RTD records FIFO : " << _rtd_recs_.size() << std::endl;
        std::size_t counter = 0;
        for (const auto& rtd_rec : _rtd_recs_) {
          out << "|   ";
          if (counter + 1 == _rtd_recs_.size()) {
            out << "`-- ";
          } else {
            out << "|-- ";
          }
          out << rtd_rec;
          out << std::endl;
          counter++;
        }
        out << "|-- Front trigger ID : " << _front_trigger_id_ << std::endl;
        out << "`-- Terminated       : " << std::boolalpha << _terminated_
            << std::endl;
        out << std::endl;
        out_ << out.str();
      }

    private:
      int32_t _front_trigger_id_ = -1;
      std::deque<snfee::io::rtd_record> _rtd_recs_;
      bool _terminated_ = false;
    };

    /// \brief Pimpl-ized private resources
    ///
    ///                               imtx #0
    ///           +------------+     +------------+
    /// [RHD]-->--| iworker #0 |-->--| ibuffer #0 |
    ///           +------------+     +------------+.
    ///                               imtx #1       \    +----------------+ omtx
    ///           +------------+     +------------+  \   |                |
    ///           +---------+
    /// [RHD]-->--| iworker #1 |-->--| ibuffer #1 |---->-|     merger |-->--|
    /// obuffer |-->--[RTD]
    ///           +------------+     +------------+  /   |                |
    ///           +---------+
    ///                 :                  :        /    +----------------+
    ///           +------------+     +------------+'
    /// [RHD]-->--| iworker #N |-->--| ibuffer #N |
    ///           +------------+     +------------+
    ///                               imtx #N
    ///
    struct builder::pimpl_type {
      std::vector<input_worker_ptr>
        iworkers; ///< Collection of RHD input workers
      std::vector<rhd_buffer>
        ibuffers; ///< Collection of buffers of input raw hit data records (RHD)
      std::vector<std::shared_ptr<std::mutex>>
        imtxs; ///< Collection of mutexes for individual access to input buffers
      std::shared_ptr<std::mutex>
        gimtx;                   ///< Mutex for access to all input buffers
      rhd2rtd_merger_ptr merger; ///< Merger of RHD records to RTD records
      rtd_buffer obuffer;        ///< Buffer of output raw trigger data (RTD)
      std::shared_ptr<std::mutex> omtx; ///< Mutex for access to output buffer
      output_worker_ptr oworker;        ///< RTD output worker

      friend struct rhd_merger;
    };

    /// \brief RHD to RTD merger
    struct rhd2rtd_merger {

      rhd2rtd_merger(builder::pimpl_type& pimpl_,
                     const int32_t run_id_,
                     const bool force_complete_rtd_ = false,
                     const datatools::logger::priority logging_ =
                       datatools::logger::PRIO_FATAL)
        : _pimpl_(pimpl_)
      {
        _logging_ = logging_;
        _force_complete_rtd_ = force_complete_rtd_;
        _run_id_ = run_id_;
      }

      /// Request stop
      void
      stop()
      {
        _stop_request_ = true;
      }

      bool
      is_stopped() const
      {
        return _stop_request_;
      }

      std::size_t
      get_rtd_records_counter() const
      {
        return _rtd_records_counter_;
      };

      int32_t
      get_minimum_trigger_id_from_input_buffers() const
      {
        snfee::data::trigger_id_comparator_type trigger_id_less;
        int32_t mintrigid = snfee::data::INVALID_TRIGGER_ID;
        bool cannot_determine = false;
        for (int i = 0; i < (int)_pimpl_.ibuffers.size(); i++) {
          auto& ibuf = _pimpl_.ibuffers[i];
          {
            std::lock_guard<std::mutex> lck(*_pimpl_.imtxs[i]);
            // DT_LOG_DEBUG(_logging_, "Input buffer #" << i << " is locked by
            // merger.");
            if (!ibuf.is_finished()) {
              // We consider only running buffers:
              int32_t next_trig_id = ibuf.get_next_poppable_trig_id();
              if (next_trig_id == snfee::data::INVALID_TRIGGER_ID) {
                // No way to determine the next trigger ID to be collected:
                cannot_determine = true;
              }
              if (!cannot_determine) {
                if (ibuf.get_front_trig_id() !=
                    snfee::data::INVALID_TRIGGER_ID) {
                  if (mintrigid == snfee::data::INVALID_TRIGGER_ID or
                      trigger_id_less(next_trig_id, mintrigid)) {
                    mintrigid = next_trig_id;
                  }
                }
              }
            }
            // DT_LOG_DEBUG(_logging_, "Input buffer #" << i << " is unlocked by
            // merger.");
          }
          if (cannot_determine) {
            mintrigid = snfee::data::INVALID_TRIGGER_ID;
            break;
          }
        }
        return mintrigid;
      };

      void
      run()
      {
        DT_LOG_TRACE_ENTERING(_logging_);
        snfee::data::trigger_id_comparator_type trigger_id_less;
        // Working RTD record:
        snfee::io::rtd_record rtd_rec;

        std::size_t nin = _pimpl_.ibuffers.size();
        bool all_input_buffers_finished = false;
        std::set<int> finished_buffers;
        _rtd_records_counter_ = 0;
        while (!_stop_request_) {

          // Extract the next trigger ID from the input buffers:
          int32_t fetchable_trig_id =
            get_minimum_trigger_id_from_input_buffers();
          bool process_input_rhd = false;
          if (fetchable_trig_id == snfee::data::INVALID_TRIGGER_ID) {
            // None of the input buffers can provide new records yet
            // DT_LOG_DEBUG(_logging_, "No fetchable trigger ID is available
            // yet!");
          } else {
            // DT_LOG_DEBUG(_logging_, "Fetchable trigger ID is : " <<
            // fetchable_trig_id);
            process_input_rhd = true;
          }

          // Manage the completeness of the current RTD for pushing in the
          // output buffer:
          bool push_current_rtd = false;
          if (process_input_rhd) {
            if (rtd_rec.get_trigger_id() != snfee::data::INVALID_TRIGGER_ID and
                trigger_id_less(rtd_rec.get_trigger_id(), fetchable_trig_id)) {
              // We already have a working RTD record and it has been completed
              // (no more records with same trigger ID are expected from the
              // input buffers), so we push it in the output buffer:
              DT_LOG_DEBUG(
                _logging_,
                "Make the current RTD pushable because input buffers cannot "
                "provide RHD records with trigger ID = "
                  << rtd_rec.get_trigger_id() << " anymore.");
              push_current_rtd = true;
            }
          } else if (all_input_buffers_finished) {
            DT_LOG_DEBUG(_logging_,
                         "Make the current RTD pushable because this is the "
                         "end of the input buffers.");
            push_current_rtd = true;
          }

          if (push_current_rtd) {
            DT_LOG_DEBUG(_logging_,
                         "Current RTD record is completed with trigger ID = "
                           << rtd_rec.get_trigger_id());
            {
              std::lock_guard<std::mutex> olck(*_pimpl_.omtx);
              DT_LOG_DEBUG(_logging_, "Output buffer is locked by merger.");
              DT_THROW_IF(_force_complete_rtd_ and
                            !rtd_rec.get_rtd().is_complete(),
                          std::logic_error,
                          "Incomplete RTD data!");
              if (!rtd_rec.get_rtd().has_trig()) {
                if (_use_mock_trig_) {
                  auto& rtd = rtd_rec.grab_rtd();
                  auto trig_ptr =
                    std::make_shared<snfee::data::trigger_record>();
                  int32_t trigger_hit_num = 0;
                  int32_t trigger_id = rtd_rec.get_trigger_id();
                  snfee::data::trigger_record::trigger_mode_type trigger_mode =
                    snfee::data::trigger_record::TRIGGER_MODE_CALO_ONLY;
                  trig_ptr->make(trigger_hit_num, trigger_id, trigger_mode);
                  rtd.set_trig(trig_ptr);
                }
              }

              _pimpl_.obuffer.push_record(rtd_rec);
              // We reset the working RTD record and trigger ID:
              DT_LOG_DEBUG(_logging_, "Reset the working RTD record...");
              rtd_rec.reset();
              _rtd_records_counter_++;
            }
            if (datatools::logger::is_debug(_logging_)) {
              _pimpl_.obuffer.print(std::cerr);
            }
            DT_LOG_DEBUG(_logging_, "Output buffer is unlocked by merger.");
          }

          if (process_input_rhd) {
            // There is some RHD to be processed:
            if (rtd_rec.get_trigger_id() == snfee::data::INVALID_TRIGGER_ID) {
              // We initiate a new RTD working record to host upcoming RHD
              // records:
              DT_LOG_DEBUG(_logging_,
                           "Make a new working RTD record with trigger ID = "
                             << fetchable_trig_id);
              rtd_rec.make_record(_run_id_, fetchable_trig_id);
            }

            DT_LOG_DEBUG(_logging_, "Loop on input buffers...");
            // Scan all input buffers:
            for (int i = 0; i < (int)_pimpl_.ibuffers.size(); i++) {
              DT_LOG_DEBUG(_logging_, "Inspect input buffer #" << i);
              {
                std::lock_guard<std::mutex> lck(*_pimpl_.imtxs[i]);
                DT_LOG_DEBUG(_logging_,
                             "Input buffer #" << i << " is locked by merger.");
                auto& ibuf = _pimpl_.ibuffers[i];
                if (datatools::logger::is_debug(_logging_)) {
                  ibuf.print(std::cerr);
                }
                // Extract all RHD records matching the working trigger ID from
                // the input buffer:
                while (!ibuf.is_empty()) {
                  if (ibuf.get_front_trig_id() != rtd_rec.get_trigger_id()) {
                    break;
                  }
                  DT_LOG_DEBUG(_logging_,
                               "Pop record from input buffer #" << i);
                  const snfee::io::rhd_record& rec = ibuf.pop_record();
                  if (datatools::logger::is_debug(_logging_)) {
                    rec.print(std::cerr);
                  }
                  DT_LOG_DEBUG(_logging_,
                               "Install the RHD record from input buffer #"
                                 << i << " into the RTD record...");
                  rtd_rec.install_rhd(rec);
                  DT_LOG_DEBUG(
                    _logging_,
                    "RHD record has been inserted in the current RTD record!");
                }
                if (datatools::logger::is_debug(_logging_)) {
                  ibuf.print(std::cerr);
                }
                DT_LOG_DEBUG(_logging_,
                             "Input buffer #" << i
                                              << " is unlocked by merger.");
              }
              if (datatools::logger::is_debug(_logging_)) {
                rtd_rec.print(std::cerr);
              }
              DT_LOG_DEBUG(_logging_, "Done with input buffers #" << i);
            } // end of scan for loop all input buffers:
          }   // process_input_rhd

          if (!all_input_buffers_finished) {
            // Count the number of finished input buffers:
            std::size_t nfinished = 0;
            // bool print_bufs = false;
            for (int i = 0; i < (int)_pimpl_.ibuffers.size(); i++) {
              std::lock_guard<std::mutex> lck(*_pimpl_.imtxs[i]);
              DT_LOG_DEBUG(_logging_,
                           "Input buffer #" << i << " is locked by merger.");
              const auto& ibuf = _pimpl_.ibuffers[i];
              if (ibuf.is_finished()) {
                nfinished++;
                if (finished_buffers.count(i) == 0U) {
                  finished_buffers.insert(i);
                  // print_bufs = true;
                  DT_LOG_NOTICE(_logging_,
                                "Input buffer #" << i << " is finished.");
                }
              }
              DT_LOG_DEBUG(_logging_,
                           "Input buffer #" << i << " is unlocked by merger.");
            }

            if (nfinished == nin) {
              all_input_buffers_finished = true;
              DT_LOG_NOTICE(_logging_, "All input buffers are finished.");
            }
          }
          if (all_input_buffers_finished and
              rtd_rec.get_trigger_id() == snfee::data::INVALID_TRIGGER_ID) {
            DT_LOG_NOTICE(_logging_,
                          "All input buffers are finished and there is no more "
                          "current RTD record. Stop!");
            stop();
          }
          if ((_rtd_records_counter_ % 100 == 0) or is_stopped()) {
            DT_LOG_NOTICE(_logging_,
                          "Merger run : " << _rtd_records_counter_
                                          << " built RTD records");
          }
          std::this_thread::yield();
        } // main while loop

        rtd_rec.reset();
        {
          std::lock_guard<std::mutex> olck(*_pimpl_.omtx);
          DT_LOG_DEBUG(_logging_, "Output buffer is terminated.");
          _pimpl_.obuffer.terminate();
        }

        DT_LOG_NOTICE(_logging_, "Merger run is stopped.");
        DT_LOG_TRACE_EXITING(_logging_);
      }

      void
      set_use_mock_trig(bool umt_)
      {
        _use_mock_trig_ = umt_;
      }

    private:
      // Configuration:
      int32_t _run_id_ = snfee::data::INVALID_RUN_ID;
      bool _force_complete_rtd_ = false;
      bool _use_mock_trig_ = false;
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;

      // Working:
      builder::pimpl_type& _pimpl_;
      bool _stop_request_ = false;           ///< Thread stop request
      std::size_t _rtd_records_counter_ = 0; ///< Counter of built RTD records
    };

    /// \brief RTD output worker
    struct output_worker {

      /// Constructor
      output_worker(
        std::mutex& omtx_,
        rtd_buffer& obuf_,
        const snfee::rtdb::builder_config::output_config_type& oconfig_,
        const datatools::logger::priority logging_ =
          datatools::logger::PRIO_FATAL)
        : _mtx_(omtx_), _buf_(obuf_)
      {
        _logging_ = logging_;
        snfee::io::multifile_data_writer::config_type writer_config;
        writer_config.filenames = oconfig_.filenames;
        writer_config.max_records_per_file = oconfig_.max_records_per_file;
        writer_config.max_total_records = oconfig_.max_total_records;
        writer_config.terminate_on_overrun = oconfig_.terminate_on_overrun;
        _pwriter_ =
          std::make_shared<snfee::io::multifile_data_writer>(writer_config);
      }

      /// Destructor
      ~output_worker()
      {
        if (_pwriter_) {
          _pwriter_.reset();
        }
      }

      /// Request stop
      void
      stop()
      {
        _stop_request_ = true;
      }

      bool
      is_stopped() const
      {
        return _stop_request_;
      }

      std::size_t
      get_records_counter() const
      {
        return _records_counter_;
      };

      std::size_t
      get_stored_records_counter() const
      {
        return _stored_records_counter_;
      };

      /// Run
      void
      run()
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
                           "Pop RTD record from the output RTD buffer...");
              _records_counter_++;
              snfee::io::rtd_record rec = _buf_.pop_record();
              if (!writer_is_terminated and _pwriter_->is_terminated()) {
                writer_is_terminated = true;
                DT_LOG_NOTICE(_logging_,
                              "Output RTD writer is now terminated.");
              }
              if (!writer_is_terminated) {
                DT_LOG_DEBUG(_logging_, "Store the RTD record.");
                _pwriter_->store(rec.get_rtd());
                _stored_records_counter_++;
                DT_LOG_DEBUG(_logging_, "RTD record is stored.");
              } else {
                // Anticipated stop because writer is terminated:
                DT_LOG_NOTICE(_logging_,
                              "Output RTD writer: anticipated stop.");
                stop();
              }
            }
            if (_buf_.is_empty() && _buf_.is_terminated()) {
              DT_LOG_DEBUG(_logging_, "Output RTD buffer is finished.");
              if (datatools::logger::is_debug(_logging_)) {
                _buf_.print(std::cerr);
              }
              DT_LOG_DEBUG(_logging_, "Request the output worker to stop.");
              stop();
            }
          }
          if (_records_counter_ % 100 == 0 or is_stopped()) {
            DT_LOG_NOTICE(_logging_,
                          "Output worker run : " << _records_counter_
                                                 << " saved RTD records");
          }
          std::this_thread::yield();
        }
        DT_LOG_NOTICE(_logging_, "Output worker run is stopped.");
        DT_LOG_TRACE_EXITING(_logging_);
      }

    private:
      std::mutex& _mtx_; ///< Mutex for access to the output RTD buffer
      rtd_buffer& _buf_; ///< Handle to the output RTD buffer
      std::shared_ptr<snfee::io::multifile_data_writer>
        _pwriter_;                 ///< Data writer
      bool _stop_request_ = false; ///< Thread stop request
      datatools::logger::priority _logging_ =
        datatools::logger::PRIO_FATAL;   ///< Logging priority
      std::size_t _records_counter_ = 0; ///< Counter of processed RTD records
      std::size_t _stored_records_counter_ =
        0; ///< Counter of stored RTD records
    };

    void
    builder::_at_init_()
    {
      _pimpl_.reset(new pimpl_type);
      pimpl_type& pimpl = *_pimpl_;

      // Ouput manager:
      DT_LOG_NOTICE(_logging_, "Instantiating the output worker...");
      pimpl.omtx = std::make_shared<std::mutex>();
      pimpl.oworker = std::make_shared<output_worker>(
        *(pimpl.omtx), pimpl.obuffer, _config_.output_config, _logging_);

      // Merger:
      DT_LOG_NOTICE(_logging_, "Instantiating the merger...");
      pimpl.merger = std::make_shared<rhd2rtd_merger>(
        *_pimpl_, _config_.run_id, _config_.force_complete_rtd, _logging_);

      // Input managers:
      {
        pimpl.gimtx = std::make_shared<std::mutex>();
        int icount = 0;
        for (const auto& iconfig : _config_.input_configs) {
          DT_LOG_NOTICE(_logging_,
                        "Instantiating the input buffer mutex #" << icount
                                                                 << "...");
          pimpl.imtxs.push_back(std::make_shared<std::mutex>());
          DT_LOG_NOTICE(
            _logging_,
            "Instantiating the input buffer #"
              << icount << " for crate '"
              << snfee::model::crate_model_label(iconfig.crate_model)
              << "'...");
          std::size_t min_popping_trig_ids = 1;
          if (_config_.accept_unsorted_records) {
            min_popping_trig_ids =
              _config_.unsorted_records_min_popping_safety_depth;
          }
          uint32_t capacity = 0;
          if (iconfig.crate_model == snfee::model::CRATE_CALORIMETER) {
            capacity = _config_.calo_rhd_buffer_capacity;
          }
          if (iconfig.crate_model == snfee::model::CRATE_TRACKER) {
            capacity = _config_.tracker_rhd_buffer_capacity;
          }
          rhd_buffer buf(icount, capacity, min_popping_trig_ids);
          pimpl.ibuffers.push_back(buf);
          icount++;
        }
      }

      {
        int icount = 0;
        for (const auto& ibuf : pimpl.ibuffers) {
          DT_LOG_NOTICE(_logging_, "Input buffer #" << icount << "...");
          ibuf.print(std::cerr);
          icount++;
        }
      }

      // Input workers:
      {
        int icount = 0;
        for (const auto& iconfig : _config_.input_configs) {
          DT_LOG_NOTICE(_logging_,
                        "Instantiating the input worker #" << icount << "...");
          auto iwrk = std::make_shared<input_worker>(icount,
                                                     *pimpl.imtxs[icount].get(),
                                                     pimpl.ibuffers[icount],
                                                     iconfig,
                                                     _logging_);
          DT_LOG_DEBUG(_logging_, "iwrk = [@" << iwrk.get() << "]");
          pimpl.iworkers.emplace_back(iwrk);
          DT_LOG_DEBUG(_logging_,
                       "Done for input worker #" << icount << " [@"
                                                 << pimpl.iworkers.back().get()
                                                 << "]...");
          if (datatools::logger::is_debug(_logging_)) {
            pimpl.iworkers.back()->print(std::cerr);
          }
          icount++;
        }
      }
    }

    void
    builder::_at_terminate_()
    {
      if (_pimpl_) {
        pimpl_type& pimpl = *_pimpl_;

        worker_results_type owResults;
        owResults.category = WORKER_OUTPUT_RTD;
        owResults.processed_records_counter1 =
          pimpl.oworker->get_records_counter();
        owResults.processed_records_counter2 =
          pimpl.oworker->get_stored_records_counter();
        _results_.push_back(owResults);

        std::size_t icount = 0;
        if (!pimpl.iworkers.empty()) {
          for (auto& iwkr : pimpl.iworkers) {

            worker_results_type iwResults;
            iwResults.category = WORKER_INPUT_RHD;
            iwResults.crate_model = _config_.input_configs[icount].crate_model;
            iwResults.processed_records_counter1 = iwkr->get_records_counter();
            _results_.push_back(iwResults);

            DT_LOG_DEBUG(_logging_,
                         "Destroying the input worker #" << icount << "...");
            iwkr.reset();
            icount++;
          }
          pimpl.iworkers.clear();
        }

        if (pimpl.merger) {
          // pimpl.merger->stop();
          DT_LOG_DEBUG(_logging_, "Destroying the merger...");
          pimpl.merger.reset();
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
      pimpl_type& pimpl = *_pimpl_;

      // Input worker threads:
      std::vector<std::thread> ithreads;
      for (std::size_t i = 0; i < _config_.input_configs.size(); i++) {
        DT_LOG_DEBUG(_logging_,
                     "Starting thread for input worker #"
                       << i << " at [@" << pimpl.iworkers[i].get() << "]...");
        input_worker& iwrk = *pimpl.iworkers[i];
        ithreads.emplace_back(std::thread(&input_worker::run, std::ref(iwrk)));
      }
      DT_LOG_DEBUG(_logging_, "All input workers have been launched.");

      // Merger thread:
      rhd2rtd_merger& merger = *pimpl.merger;
      std::thread mthread(&rhd2rtd_merger::run, std::ref(merger));

      // Output worker thread:
      output_worker& owrk = *pimpl.oworker;
      std::thread othread(&output_worker::run, std::ref(owrk));

      // Join threads:
      for (auto& ithrd : ithreads) {
        ithrd.join();
      }
      mthread.join();
      othread.join();

      DT_LOG_TRACE_EXITING(_logging_);
    }

  } // namespace rtdb
} // namespace snfee
