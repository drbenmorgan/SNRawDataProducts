// This project:
#include "red_record.h"

namespace snfee {
  namespace io {

    red_record::red_record() { return; }

    void
    red_record::make_record(const int32_t run_id_,
                            const int32_t event_id_,
                            const snfee::data::timestamp& reference_time_)
    {
      _red_ = std::make_shared<snfee::data::raw_event_data>();
      _red_->set_run_id(run_id_);
      _red_->set_event_id(event_id_);
      _red_->set_reference_time(reference_time_);
      return;
    }

    void
    red_record::reset()
    {
      _red_.reset();
      return;
    }

    bool
    red_record::has_record() const
    {
      return _red_.get() != nullptr;
    }

    int32_t
    red_record::get_event_id() const
    {
      DT_THROW_IF(!has_record(), std::logic_error, "No RED record!");
      return _red_->get_event_id();
    }

    snfee::data::raw_event_data&
    red_record::grab_red()
    {
      DT_THROW_IF(!has_record(), std::logic_error, "No RED record!");
      return *_red_.get();
    }

    const snfee::data::raw_event_data&
    red_record::get_red() const
    {
      DT_THROW_IF(!has_record(), std::logic_error, "No RED record!");
      return *_red_.get();
    }

    const std::shared_ptr<snfee::data::raw_event_data>&
    red_record::get_red_ptr() const
    {
      return _red_;
    }

    void
    red_record::install_rtd(
      const std::shared_ptr<const snfee::data::raw_trigger_data>& rtd_ptr_,
      const snfee::data::timestamp& timeshift_)
    {
      DT_THROW_IF(!has_record(), std::logic_error, "No RED record!");
      _red_->add_rtd(timeshift_, rtd_ptr_);
      return;
    }

    void
    red_record::print(std::ostream& out_) const
    {
      std::ostringstream obuffer;
      obuffer << "RED output record: " << std::endl;
      obuffer << "`-- "
              << "RED record : ";
      if (_red_) {
        obuffer << _red_.get();
      } else {
        obuffer << "<none>";
      }
      obuffer << std::endl;
      out_ << obuffer.str();
      return;
    }

    // friend
    std::ostream&
    operator<<(std::ostream& out_, const red_record& rec_)
    {
      std::ostringstream out;
      out << "RED={payload=";
      if (!rec_._red_) {
        out << "none";
      } else {
        out << rec_._red_.get();
      }
      out << '}';
      out_ << out.str();
      return out_;
    }

  } // namespace io
} // namespace snfee
