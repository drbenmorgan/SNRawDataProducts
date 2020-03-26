// This project:
#include "rhd_sorter.h"

// Standard Library:
#include <iomanip>
#include <string>
#include <typeinfo>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
#include <boost/circular_buffer.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

// This project:
//#include "raw_record_flow_statistics.h"
#include <snfee/data/calo_hit_record.h>
#include <snfee/data/raw_record_interface.h>
#include <snfee/data/tracker_hit_record.h>
#include <snfee/data/trigger_record.h>
#include <snfee/io/multifile_data_reader.h>
#include <snfee/io/multifile_data_writer.h>

namespace snfee {
  namespace io {

    /// \brief Comparator of raw records with respect to their trigger IDs
    ///
    /// The comparison takes into account the range of valid trigger IDs
    /// and possible overflow values above max trigger ID (0xC00000).
    struct raw_record_comparator_type {

      bool
      operator()(const snfee::data::raw_record_ptr& lhs_,
                 const snfee::data::raw_record_ptr& rhs_) const
      {
        snfee::data::trigger_id_comparator_type tid_comp;
        return tid_comp(lhs_->get_trigger_id(), rhs_->get_trigger_id());
      }
    };

    /// \brief Running mode
    enum mode_type {
      MODE_UNDEF = 0x0,      ///< Undefined/invalid running mode
      MODE_EVALUATION = 0x1, ///< Sort running mode
      MODE_SORT = 0x2        ///< Evaluation running mode
    };

    // Private implementation : working data structures
    struct rhd_sorter::pimpl_type {
      std::unique_ptr<multifile_data_reader> reader;
      std::unique_ptr<multifile_data_writer> writer;
      boost::circular_buffer<snfee::data::raw_record_ptr> cbuffer;
      mode_type mode = MODE_UNDEF;
      bool no_store = false;
      std::size_t input_hit_counter = 0;
      std::size_t output_hit_counter = 0;
      std::size_t output_trigger_counter = 0;
      int32_t max_output_trigger_id = -1;
      int32_t current_trigger_id = -1;
      // std::unique_ptr<raw_record_flow_statistics> stats;
      std::string report_filename;

      void dump_buffer(std::ostream& out_) const;
    };

    rhd_sorter::rhd_sorter() { _pimpl_.reset(new pimpl_type); }

    rhd_sorter::~rhd_sorter() { _pimpl_.reset(); }

    datatools::logger::priority
    rhd_sorter::get_logging() const
    {
      return _logging_;
    }

    void
    rhd_sorter::set_logging(const datatools::logger::priority p_)
    {
      _logging_ = p_;
    }

    void
    rhd_sorter::set_config(const config_type& cfg_)
    {
      DT_THROW_IF(
        is_initialized(), std::logic_error, "Sorter is already initialized!");
      _config_ = cfg_;
    }

    bool
    rhd_sorter::is_initialized() const
    {
      return _initialized_;
    }

    void
    rhd_sorter::initialize()
    {
      DT_THROW_IF(
        is_initialized(), std::logic_error, "Sorter is already initialized!");

      _pimpl_->no_store = false;
      if (_config_.no_store) {
        _pimpl_->no_store = true;
      }

      // Fetch running mode:
      if (!_config_.mode_label.empty()) {
        DT_LOG_DEBUG(_logging_, "Mode label : '" << _config_.mode_label << "'");
        if (_config_.mode_label == "sort") {
          DT_LOG_DEBUG(_logging_, "Running mode : 'sort'");
          _pimpl_->mode = MODE_SORT;
        } else if (_config_.mode_label == "evaluation") {
          _pimpl_->mode = MODE_EVALUATION;
          DT_LOG_DEBUG(_logging_, "Running mode : 'evaluation'");
        } else {
          DT_THROW(std::logic_error, "Invalid mode!");
        }
      }

      // Default running mode:
      if (_pimpl_->mode == MODE_UNDEF) {
        DT_LOG_DEBUG(_logging_, "Running default 'sort' mode...");
        _pimpl_->mode = MODE_SORT;
      }

      if (!_config_.report_filename.empty()) {
        _pimpl_->report_filename = _config_.report_filename;
      }

      if (_pimpl_->mode == MODE_EVALUATION) {
        _pimpl_->no_store = true;
        //_pimpl_->stats.reset(
        //  new raw_record_flow_statistics(_config_.evaluation_buffer_size));
        if (_pimpl_->report_filename.empty()) {
          _pimpl_->report_filename = "evaluation.report";
          DT_LOG_DEBUG(_logging_,
                       "Evaluation report file : '" << _pimpl_->report_filename
                                                    << "'");
        }
      } else if (_pimpl_->mode == MODE_SORT) {
        if (_pimpl_->report_filename.empty()) {
          _pimpl_->report_filename = "sort.report";
          DT_LOG_DEBUG(_logging_,
                       "Sort report file : '" << _pimpl_->report_filename
                                              << "'");
        }
      }

      // RHD reader:
      multifile_data_reader::config_type reader_cfg;

      if (!_config_.input_rhd_listname.empty()) {
        // Read a file containing a list of RHD input filenames:
        std::string listname = _config_.input_rhd_listname;
        datatools::fetch_path_with_env(listname);
        std::ifstream finput_list(listname.c_str());
        DT_THROW_IF(!finput_list,
                    std::logic_error,
                    "Cannot open input files list filename!");
        while (finput_list and !finput_list.eof()) {
          std::string line;
          std::getline(finput_list, line);
          boost::trim(line);
          if (line.empty()) {
            continue;
          }
          std::istringstream ins(line);
          std::string filename;
          ins >> filename >> std::ws;
          if (!filename.empty() and filename[0] != '#') {
            reader_cfg.filenames.push_back(filename);
          }
        }
      }
      // Add any explicit RHD input filenames:
      for (const auto& input_rhd_filename : _config_.input_rhd_filenames) {
        reader_cfg.filenames.push_back(input_rhd_filename);
      }
      _pimpl_->reader.reset(new multifile_data_reader(reader_cfg));

      bool with_writer = false;
      DT_LOG_DEBUG(_logging_,
                   "no_store = " << std::boolalpha << _config_.no_store);
      if (_pimpl_->mode == MODE_SORT && !_config_.no_store) {
        with_writer = true;
      }
      if (with_writer) {
        DT_LOG_DEBUG(_logging_, "Instantiating writer...");
        // RHD writer:
        multifile_data_writer::config_type writer_cfg;
        if (_config_.max_records_per_file != 0) {
          writer_cfg.max_records_per_file = _config_.max_records_per_file;
        }
        // if (_config_.max_total_records != 0) {
        //   writer_cfg.max_total_records = _config_.max_total_records;
        // }
        if (!_config_.output_rhd_listname.empty()) {
          // Read a file containing a list of RHD output filenames:
          std::string listname = _config_.output_rhd_listname;
          datatools::fetch_path_with_env(listname);
          std::ifstream finput_list(listname.c_str());
          DT_THROW_IF(!finput_list,
                      std::logic_error,
                      "Cannot open output files list filename!");
          while (finput_list and !finput_list.eof()) {
            std::string line;
            std::getline(finput_list, line);
            boost::trim(line);
            if (line.empty()) {
              continue;
            }
            std::istringstream ins(line);
            std::string filename;
            ins >> filename >> std::ws;
            if (!filename.empty() and filename[0] != '#') {
              writer_cfg.filenames.push_back(filename);
            }
          }
        }
        // Add any explicit RHD output filenames:
        for (const auto& output_rhd_filename : _config_.output_rhd_filenames) {
          writer_cfg.filenames.push_back(output_rhd_filename);
        }
        _pimpl_->writer.reset(new multifile_data_writer(writer_cfg));
      }

      // Set the capacity of the sorting buffer
      _pimpl_->cbuffer.set_capacity(_config_.sort_buffer_size);

      _initialized_ = true;
    }

    void
    rhd_sorter::_output_sorted_record_(const snfee::data::raw_record_ptr& ph_)
    {
      // Statistics:
      _pimpl_->output_hit_counter++;
      int32_t tid = ph_->get_trigger_id();

      // Record and check the maximum trigger ID:
      if (_pimpl_->max_output_trigger_id < 0) {
        _pimpl_->max_output_trigger_id = tid;
      } else {
        snfee::data::trigger_id_comparator_type trigger_id_less;
        if (trigger_id_less(tid, _pimpl_->max_output_trigger_id)) {
          DT_THROW(
            std::logic_error,
            "Output a record #"
              << _pimpl_->output_hit_counter << " with trigger ID=[" << tid
              << "]"
              << " which is less than the maximum trigger ID=["
              << _pimpl_->max_output_trigger_id
              << "] processed so far! Sort buffer is probably too small!");
        }
        if (trigger_id_less(_pimpl_->max_output_trigger_id, tid)) {
          _pimpl_->max_output_trigger_id = tid;
        }
      }
      if (_pimpl_->current_trigger_id < 0) {
        _pimpl_->current_trigger_id = tid;
      } else {
        if (tid != _pimpl_->current_trigger_id) {
          _pimpl_->current_trigger_id = tid;
          _pimpl_->output_trigger_counter++;
        }
      }
      if (_config_.log_modulo != 0U) {
        if ((_pimpl_->output_hit_counter % _config_.log_modulo) == 0) {
          std::clog << "#sorted hits=" << _pimpl_->output_hit_counter << "\n";
          std::clog << "#sorted triggers=" << _pimpl_->output_trigger_counter
                    << "\n";
        }
      }

      if (_pimpl_->writer) {
        if (ph_) {
          const snfee::data::raw_record_interface* rri = ph_.get();
          DT_LOG_DEBUG(_logging_, "Output record type=" << typeid(*rri).name());
        } else {
          DT_THROW(std::logic_error,
                   "No raw record to be stored! This is a bug!");
        }
        if (typeid(*ph_.get()) == typeid(snfee::data::calo_hit_record)) {

          const auto& new_calo_hit =
            dynamic_cast<const snfee::data::calo_hit_record&>(*ph_.get());
          _pimpl_->writer->store(new_calo_hit);

        } else if (typeid(*ph_.get()) ==
                   typeid(snfee::data::tracker_hit_record)) {

          const auto& new_tracker_hit =
            dynamic_cast<const snfee::data::tracker_hit_record&>(*ph_.get());
          _pimpl_->writer->store(new_tracker_hit);

        } else if (typeid(*ph_.get()) == typeid(snfee::data::trigger_record)) {

          auto& new_trigger_rec =
            dynamic_cast<const snfee::data::trigger_record&>(*ph_.get());
          _pimpl_->writer->store(new_trigger_rec);
        }
      }
    }

    void
    rhd_sorter::_finalize_()
    {
      DT_LOG_DEBUG(_logging_, "Sort the remaining hits in the buffer...");
      raw_record_comparator_type comparator;
      std::sort(_pimpl_->cbuffer.begin(), _pimpl_->cbuffer.end(), comparator);
      DT_LOG_DEBUG(_logging_, "Sorted final buffer: ");
      if (datatools::logger::is_debug(_logging_)) {
        _pimpl_->dump_buffer(std::cerr);
      }
      DT_LOG_DEBUG(_logging_, "Empty the buffer...");
      size_t npop = _pimpl_->cbuffer.size();
      DT_LOG_DEBUG(_logging_,
                   "About to remove the remaining "
                     << npop << " records from the buffer...");
      for (size_t i = 0; i < npop; i++) {
        snfee::data::raw_record_ptr prec = _pimpl_->cbuffer.front();
        _pimpl_->cbuffer.pop_front();
        _output_sorted_record_(prec);
        DT_LOG_DEBUG(_logging_,
                     "Pop record with trigger ID=[" << prec->get_trigger_id()
                                                    << "]");
      }
      if (_config_.log_modulo != 0U) {
        if ((_pimpl_->output_hit_counter % _config_.log_modulo) == 0) {
          std::clog << "#sorted hits=" << _pimpl_->output_hit_counter << "\n";
          std::clog << "#sorted triggers=" << _pimpl_->output_trigger_counter
                    << "\n";
        }
      }
    }

    void
    rhd_sorter::_input_unsorted_record_(snfee::data::raw_record_ptr& ph_)
    {
      if (ph_) {
        snfee::data::raw_record_interface* rri = ph_.get();
        DT_LOG_DEBUG(_logging_,
                     "Input raw record type = '" << typeid(*rri).name()
                                                 << "'\n");
      }
      if (_pimpl_->reader->record_tag_is(
            snfee::data::calo_hit_record::SERIAL_TAG)) {
        if (!ph_ ||
            typeid(*ph_.get()) != typeid(snfee::data::calo_hit_record)) {
          ph_ = std::make_shared<snfee::data::calo_hit_record>();
        } else {
          ph_->invalidate();
        }
        auto& new_calo_hit =
          dynamic_cast<snfee::data::calo_hit_record&>(*ph_.get());
        _pimpl_->reader->load(new_calo_hit);
      } else if (_pimpl_->reader->record_tag_is(
                   snfee::data::tracker_hit_record::SERIAL_TAG)) {
        if (!ph_ ||
            typeid(*ph_.get()) != typeid(snfee::data::tracker_hit_record)) {
          ph_ = std::make_shared<snfee::data::tracker_hit_record>();
        } else {
          ph_->invalidate();
        }
        auto& new_tracker_hit =
          dynamic_cast<snfee::data::tracker_hit_record&>(*ph_.get());
        _pimpl_->reader->load(new_tracker_hit);
      } else if (_pimpl_->reader->record_tag_is(
                   snfee::data::trigger_record::SERIAL_TAG)) {
        if (!ph_ || typeid(*ph_.get()) != typeid(snfee::data::trigger_record)) {
          ph_ = std::make_shared<snfee::data::trigger_record>();
        } else {
          ph_->invalidate();
        }
        ph_ = std::make_shared<snfee::data::trigger_record>();
        auto& new_trigger_rec =
          dynamic_cast<snfee::data::trigger_record&>(*ph_.get());
        _pimpl_->reader->load(new_trigger_rec);
      }
      _pimpl_->input_hit_counter++;
    }

    void
    rhd_sorter::pimpl_type::dump_buffer(std::ostream& out_) const
    {
      out_ << "Circular buffer: " << std::endl;
      out_ << "|-- Capacity : " << this->cbuffer.capacity() << std::endl;
      out_ << "|-- Size     : " << this->cbuffer.size() << std::endl;
      out_ << "|   " << std::setw(8) << "Position" << ' ' << std::setw(8)
           << "Hit num." << ' ' << std::setw(8) << "Trig. ID" << ' '
           << std::endl;
      int cnt = 0;
      for (const auto& ph : this->cbuffer) {
        int pos = this->input_hit_counter - (this->cbuffer.size() - cnt);
        out_ << "|   " << std::setw(8) << pos << ' ' << std::setw(8)
             << ph->get_hit_num() << ' ' << std::setw(8) << ph->get_trigger_id()
             << ' ' << std::endl;
        cnt++;
      }
      if (!this->cbuffer.empty()) {
        out_ << "|-- Front : " << this->cbuffer.front()->get_hit_num()
             << std::endl;
        out_ << "|-- Back  : " << this->cbuffer.back()->get_hit_num()
             << std::endl;
      }
      out_ << "|-- Empty : " << std::boolalpha << this->cbuffer.empty()
           << std::endl;
      out_ << "`-- Full  : " << std::boolalpha << this->cbuffer.full()
           << std::endl;
    }

    void
    rhd_sorter::_process_data_(const snfee::data::raw_record_ptr& ph_)
    {
      DT_LOG_DEBUG(_logging_, "Process a new raw record...");
      DT_LOG_DEBUG(_logging_,
                   "Push the new raw record in the buffer with trigger ID=["
                     << ph_->get_trigger_id() << "]");

      _pimpl_->cbuffer.push_back(ph_);
      DT_LOG_DEBUG(_logging_, "Buffer size = " << _pimpl_->cbuffer.size());
      if (datatools::logger::is_debug(_logging_)) {
        _pimpl_->dump_buffer(std::cerr);
      }
      if (_pimpl_->cbuffer.full()) {
        DT_LOG_DEBUG(_logging_, "Sorting the full buffer...");
        raw_record_comparator_type comparator;
        std::sort(_pimpl_->cbuffer.begin(), _pimpl_->cbuffer.end(), comparator);
        DT_LOG_DEBUG(_logging_, "Sorted full buffer: ");
        if (datatools::logger::is_debug(_logging_)) {
          _pimpl_->dump_buffer(std::cerr);
        }
        DT_LOG_DEBUG(_logging_, "Empty the first half of the buffer...");
        size_t npop = (_pimpl_->cbuffer.size() / 2);
        DT_LOG_DEBUG(_logging_,
                     "About to remove "
                       << npop
                       << " records from the first half of the buffer...");
        for (size_t i = 0; i < npop; i++) {
          snfee::data::raw_record_ptr prec = _pimpl_->cbuffer.front();
          _pimpl_->cbuffer.pop_front();
          _output_sorted_record_(prec);
          DT_LOG_DEBUG(_logging_,
                       "Pop record with trigger ID=[" << prec->get_trigger_id()
                                                      << "]");
        }
        DT_LOG_DEBUG(_logging_,
                     "New buffer size = " << _pimpl_->cbuffer.size());
        if (datatools::logger::is_debug(_logging_)) {
          _pimpl_->dump_buffer(std::cerr);
        }
      }
    }

    void
    rhd_sorter::run()
    {
      DT_THROW_IF(
        !is_initialized(), std::logic_error, "Sorter is not initialized!");
      while (_pimpl_->reader->has_record_tag()) {
        snfee::data::raw_record_ptr p_hit;
        _input_unsorted_record_(p_hit);
        _process_data_(p_hit);
        if (_pimpl_->mode == MODE_SORT) {
          if (_pimpl_->writer && _pimpl_->writer->is_terminated()) {
            break;
          }
        }
      }
      _finalize_();
      if (_pimpl_->mode == MODE_EVALUATION) {
        if (!_pimpl_->report_filename.empty()) {
          std::string fn = _pimpl_->report_filename;
          datatools::fetch_path_with_env(fn);
          std::ofstream freport(fn);
          DT_THROW_IF(!freport,
                      std::logic_error,
                      "Cannot open evaluation report file '" << fn << "'!");
          freport << "#@evaluation_buffer_size="
                  << _config_.evaluation_buffer_size << std::endl;
          freport.close();
        }
      }
      if (_pimpl_->mode == MODE_SORT) {
        if (!_pimpl_->report_filename.empty()) {
          std::string fn = _pimpl_->report_filename;
          datatools::fetch_path_with_env(fn);
          std::ofstream freport(fn);
          DT_THROW_IF(!freport,
                      std::logic_error,
                      "Cannot open sort report file '" << fn << "'!");
          freport << "#@sort_buffer_size=" << _config_.sort_buffer_size
                  << std::endl;
          freport << "#@input_hit_counter=" << _pimpl_->input_hit_counter
                  << std::endl;
          freport << "#@output_hit_counter=" << _pimpl_->output_hit_counter
                  << std::endl;
          freport << "#@output_trigger_counter="
                  << _pimpl_->output_trigger_counter << std::endl;
          freport << "#@max_output_trigger_id="
                  << _pimpl_->max_output_trigger_id << std::endl;
          freport.close();
        }
      }
    }

    void
    rhd_sorter::terminate()
    {
      DT_THROW_IF(
        !is_initialized(), std::logic_error, "Sorter is not initialized!");
      _initialized_ = false;
      if (_pimpl_->writer) {
        _pimpl_->writer.reset();
      }
      if (_pimpl_->reader) {
        _pimpl_->reader.reset();
      }
    }

  } // namespace io
} // namespace snfee
