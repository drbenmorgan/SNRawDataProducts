// This project:
#include <snfee/data/trigger_record.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:

namespace snfee {
  namespace data {

    DATATOOLS_SERIALIZATION_IMPLEMENTATION(trigger_record,
                                           "snfecore::trigger_record")

    // static
    std::string
    trigger_record::trigger_mode_label(const trigger_mode_type trigger_mode_)
    {
      switch (trigger_mode_) {
        case TRIGGER_MODE_CALO_ONLY:
          return std::string("calo_only");
        case TRIGGER_MODE_CALO_TRACKER_TIME_COINC:
          return std::string("calo_tracker_time_coincidence");
        case TRIGGER_MODE_CARACO:
          return std::string("CARACO");
        case TRIGGER_MODE_OPEN_DELAYED:
          return std::string("open_delayed");
        case TRIGGER_MODE_APE:
          return std::string("APE");
        case TRIGGER_MODE_DAVE:
          return std::string("DAVE");
        case TRIGGER_MODE_SCREENING:
          return std::string("screening");
        default:;
      }
      return std::string("");
    }

    trigger_record::trigger_record() { return; }

    trigger_record::~trigger_record() { return; }

    bool
    trigger_record::is_complete() const
    {
      if (_hit_num_ == INVALID_NUMBER)
        return false;
      if (_trigger_id_ == INVALID_TRIGGER_ID)
        return false;
      if (_trigger_mode_ == TRIGGER_MODE_INVALID)
        return false;
      // if (_l2_clocktick_1600ns_ == 0) return false;
      if (_trigger_mode_ == TRIGGER_MODE_APE or
          _trigger_mode_ == TRIGGER_MODE_DAVE) {
        if (_progenitor_trigger_id_ <= TRIGGER_MODE_INVALID)
          return false;
      }
      return true;
    }

    // virtual
    void
    trigger_record::print_tree(
      std::ostream& out_,
      const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      if (popts.title.length()) {
        out_ << popts.indent << popts.title << std::endl;
      }

      out_ << popts.indent << tag << "Hit number : " << _hit_num_ << std::endl;

      out_ << popts.indent << tag << "Trigger ID : " << _trigger_id_
           << std::endl;

      out_ << popts.indent << tag << "Trigger mode : '"
           << trigger_mode_label(_trigger_mode_) << "'" << std::endl;

      out_ << popts.indent << tag
           << "Progenitor trigger ID : " << _progenitor_trigger_id_
           << std::endl;

      out_ << popts.indent << tag
           << "L2 clocktick (1600 ns) : " << _l2_clocktick_1600ns_ << std::endl;

      out_ << popts.indent << inherit_tag(popts.inherit)
           << "Complete : " << std::boolalpha << is_complete() << std::endl;

      return;
    }

    void
    trigger_record::set_hit_num(const int32_t hit_num_)
    {
      _hit_num_ = hit_num_;
      return;
    }

    int32_t
    trigger_record::get_hit_num() const
    {
      return _hit_num_;
    }

    void
    trigger_record::set_trigger_id(const int32_t tid_)
    {
      DT_THROW_IF(tid_ > MAX_TRIGGER_ID,
                  std::logic_error,
                  "Invalid trigger ID=[" << tid_ << "]!");
      if (tid_ < 0) {
        _trigger_id_ = INVALID_TRIGGER_ID;
      } else {
        _trigger_id_ = tid_;
      }
      return;
    }

    int32_t
    trigger_record::get_trigger_id() const
    {
      return _trigger_id_;
    }

    trigger_record::trigger_mode_type
    trigger_record::get_trigger_mode() const
    {
      return _trigger_mode_;
    }

    void
    trigger_record::set_progenitor_trigger_id(const int32_t ptid_)
    {
      DT_THROW_IF(ptid_ > MAX_TRIGGER_ID,
                  std::logic_error,
                  "Invalid progenitor trigger ID=[" << ptid_ << "]!");
      if (ptid_ < 0) {
        _progenitor_trigger_id_ = INVALID_TRIGGER_ID;
      } else {
        _progenitor_trigger_id_ = ptid_;
      }
      return;
    }

    bool
    trigger_record::has_progenitor_trigger_id() const
    {
      return _progenitor_trigger_id_ != INVALID_TRIGGER_ID;
    }

    int32_t
    trigger_record::get_progenitor_trigger_id() const
    {
      return _progenitor_trigger_id_;
    }

    void
    trigger_record::set_l2_clocktick_1600ns(const uint32_t l2_clocktick_1600ns_)
    {
      _l2_clocktick_1600ns_ = l2_clocktick_1600ns_;
      return;
    }

    uint32_t
    trigger_record::get_l2_clocktick_1600ns() const
    {
      return _l2_clocktick_1600ns_;
    }

    timestamp
    trigger_record::convert_timestamp() const
    {
      timestamp ts;
      if (_l2_clocktick_1600ns_ == INVALID_CLOCKTICK) {
        ts.set_clock(CLOCK_UNDEF);
        ts.set_ticks(INVALID_TICKS);
      } else {
        // Convert to a 40 MHz wall clock tick:
        ts.set_clock(CLOCK_40MHz);
        ts.set_ticks(64 * _l2_clocktick_1600ns_);
      }
      return ts;
    }

    void
    trigger_record::make(const int32_t trigger_hit_num_,
                         const int32_t trigger_id_,
                         const trigger_mode_type trigger_mode_)
    {
      DT_THROW_IF(trigger_hit_num_ <= INVALID_NUMBER,
                  std::logic_error,
                  "Invalid hit number [" << trigger_hit_num_ << "]!");
      _hit_num_ = trigger_hit_num_;
      DT_THROW_IF(trigger_id_ <= INVALID_TRIGGER_ID,
                  std::logic_error,
                  "Invalid trigger ID [" << trigger_id_ << "]!");
      set_trigger_id(trigger_id_);
      _trigger_mode_ = trigger_mode_;
      return;
    }

    void
    trigger_record::invalidate()
    {
      _hit_num_ = INVALID_NUMBER;
      _trigger_id_ = INVALID_TRIGGER_ID;
      _trigger_mode_ = TRIGGER_MODE_INVALID;
      _l2_clocktick_1600ns_ = INVALID_CLOCKTICK;
      _progenitor_trigger_id_ = INVALID_TRIGGER_ID;

      return;
    }

  } // namespace data
} // namespace snfee
