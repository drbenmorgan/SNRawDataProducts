// This project:
#include <snfee/data/raw_event_data.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/data/utils.h>
#include <snfee/utils.h>

namespace snfee {
  namespace data {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(raw_event_data,
                                           "snfee::data::raw_event_data")

    bool
    raw_event_data::is_complete() const
    {
      if (!has_run_id()) {
        return false;
      }
      if (!has_event_id()) {
        return false;
      }
      if (!has_reference_time()) {
        return false;
      }
      if (_rtd_pack_.empty()) {
        return false;
      }
      return true;
    }

    void
    raw_event_data::invalidate()
    {
      _run_id_ = INVALID_EVENT_ID;
      _event_id_ = INVALID_EVENT_ID;
      _reference_time_.invalidate();
      _rtd_pack_.clear();
    }

    bool
    raw_event_data::has_run_id() const
    {
      return _run_id_ != INVALID_RUN_ID;
    }

    int32_t
    raw_event_data::get_run_id() const
    {
      return _run_id_;
    }

    void
    raw_event_data::set_run_id(const int32_t rid_)
    {
      _run_id_ = rid_;
    }

    bool
    raw_event_data::has_event_id() const
    {
      return _event_id_ != INVALID_EVENT_ID;
    }

    int32_t
    raw_event_data::get_event_id() const
    {
      return _event_id_;
    }

    void
    raw_event_data::set_event_id(const int32_t tid_)
    {
      _event_id_ = tid_;
    }

    bool
    raw_event_data::has_reference_time() const
    {
      return _reference_time_.is_valid();
    }

    const timestamp&
    raw_event_data::get_reference_time() const
    {
      return _reference_time_;
    }

    void
    raw_event_data::set_reference_time(const timestamp& ref_time_)
    {
      _reference_time_ = ref_time_;
    }

    void
    raw_event_data::add_rtd(const timestamp& time_shift_,
                            const const_raw_trigger_data_ptr& rtd_ptr_)
    {
      DT_THROW_IF(!time_shift_.is_valid(),
                  std::logic_error,
                  "Invalid time shift " << time_shift_ << " for RTD record!");
      if (!_rtd_pack_.empty()) {
        if (time_shift_ < _rtd_pack_.back().time_shift) {
          DT_THROW(std::logic_error,
                   "Invalid time ordering (" << time_shift_ << "<"
                                             << _rtd_pack_.back().time_shift
                                             << " of RTD record!");
        }
      }
      rtd_record_type dummy;
      _rtd_pack_.push_back(dummy);
      _rtd_pack_.back().time_shift = time_shift_;
      _rtd_pack_.back().rtd = rtd_ptr_;
    }

    std::size_t
    raw_event_data::get_number_of_rtd() const
    {
      return _rtd_pack_.size();
    }

    void
    raw_event_data::fetch_rtd(int index_,
                              timestamp& time_shift_,
                              const_raw_trigger_data_ptr& rtd_ptr_) const
    {
      DT_THROW_IF(index_ < 0 or index_ >= (int)_rtd_pack_.size(),
                  std::range_error,
                  "Invalid RTD pack index!");
      const auto& found = _rtd_pack_[index_];
      time_shift_ = found.time_shift;
      rtd_ptr_ = found.rtd;
    }

    // virtual
    void
    raw_event_data::print_tree(
      std::ostream& out_,
      const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length() != 0U) {
        out_ << popts.indent << popts.title << std::endl;

        out_ << popts.indent << tag << "Run ID : " << _run_id_ << std::endl;
      }

      out_ << popts.indent << tag << "Event ID : " << _event_id_ << std::endl;

      out_ << popts.indent << tag << "Reference time : " << _reference_time_
           << std::endl;

      out_ << popts.indent << tag << "RTD pack : " << _rtd_pack_.size()
           << std::endl;
      for (std::size_t i = 0; i < _rtd_pack_.size(); i++) {
        const rtd_record_type& rtd_record = _rtd_pack_[i];
        std::ostringstream indent2;
        indent2 << popts.indent << skip_tag;
        out_ << popts.indent << skip_tag;
        if (i + 1 == _rtd_pack_.size()) {
          out_ << last_tag;
          indent2 << last_skip_tag;
        } else {
          out_ << tag;
          indent2 << skip_tag;
        }
        out_ << "RTD record #" << i << " : " << std::endl;
        out_ << indent2.str() << tag << "Time shift = " << rtd_record.time_shift
             << std::endl;
        boost::property_tree::ptree thit_opts;
        thit_opts.put("indent", indent2.str());
        rtd_record.rtd->print_tree(out_, thit_opts);
      }

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Complete : " << std::boolalpha << is_complete() << std::endl;

    }

  } // namespace data
} // namespace snfee
