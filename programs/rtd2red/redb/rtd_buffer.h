// This project:
#include <snfee/redb/rtd_buffer.h>

namespace snfee {
  namespace redb {

    rtd_buffer::rtd_buffer(const uint32_t capacity_,
                           const std::size_t min_popping_trig_ids_)
    {
      _capacity_ = capacity_;
      _min_popping_trig_ids_ = min_popping_trig_ids_;
      return;
    }

    rtd_buffer::~rtd_buffer() { return; }

    std::size_t
    rtd_buffer::size() const
    {
      return _rtd_recs_.size();
    }

    std::size_t
    rtd_buffer::get_capacity() const
    {
      return _capacity_;
    }

    /// Reset the RTD buffer
    void
    rtd_buffer::reset()
    {
      _rtd_recs_.clear();
      _front_trig_id_ = snfee::data::INVALID_TRIGGER_ID;
      return;
    }

    bool
    rtd_buffer::is_finished() const
    {
      return is_empty() and is_terminated();
    }

    int32_t
    rtd_buffer::get_front_trig_id() const
    {
      return _front_trig_id_;
    }

    bool
    rtd_buffer::can_push() const
    {
      if (_capacity_ > 0 and _rtd_recs_.size() >= _capacity_) {
        return false;
      }
      return true;
    }

    void
    rtd_buffer::insert_record(const snfee::io::rtd_record rtd_rec_)
    {
      // snfee::data::trigger_id_comparator_type trigger_id_less;
      int32_t new_trig_id = rtd_rec_.get_trigger_id();
      if (_rtd_recs_.size() == 0) {
        // RTD records =      [ ]
        // New record  = [6]---^
        _rtd_recs_.push_back(rtd_rec_);
        _front_trig_id_ = new_trig_id;
      } else {
        /*
        // Protect against unsorted input:
        if (trigger_id_less(new_trig_id, _front_trig_id_)) {
        // Unsorted case:
        // RTD records =      [
        ][4][4][4][5][5][6][6][6][6][7][7][7][9][9][9][9][9]
        // New record  = [3]---^
        DT_THROW(std::logic_error,
        "Unsorted input RTD: new trigger ID=" << new_trig_id << "<"
        << "front trigger ID=" <<  _front_trig_id_ << "! This is a bug!");
        //// _rtd_recs_.push_front(rtd_rec_);
        //// _front_trig_id_ = new_trig_id;
        }
        if (trigger_id_less(new_trig_id, *_trig_ids_.rbegin())) {
        // Unsorted case (rtd_rec_.get_trigger_id() < *_trig_ids_.rbegin() :
        // RTD records = [4][4][4][5][5][6][6][6][6][7][7][7][9][9][9][9][9]
        // New record  = [6]-------------^                    ^
        // New record  = [8]----------------------------------'
        DT_THROW(std::logic_error,
        "Unsorted input RTD: new trigger ID=" << new_trig_id << "<"
        << "last trigger ID=" << *_trig_ids_.rbegin() << "! This is a bug!");
        //// std::deque<snfee::io::rtd_record>::iterator insert_iter =
        _rtd_recs_.end();
        //// for (std::deque<snfee::io::rtd_record>::iterator it =
        _rtd_recs_.begin();
        ////      it != _rtd_recs_.end();
        ////      it++) {
        ////   //if (it->get_trigger_id() >= new_trig_id) {
        ////   if (! trigger_id_less(it->get_trigger_id(), new_trig_id)) {
        ////     insert_iter = it;
        ////     break;
        ////   }
        //// }
        //// _rtd_recs_.insert(insert_iter, rtd_rec_);
        }
        */
        // Usual case:
        // RTD records = [4][4][4][5][5][6][6][ ]
        // New record  = [6]-------------------^
        // New record  = [8]-------------------'
        _rtd_recs_.push_back(rtd_rec_);
      }
      _trig_ids_.insert(new_trig_id);
      return;
    }

    snfee::io::rtd_record
    rtd_buffer::pop_record()
    {
      DT_THROW_IF(
        is_empty(), std::logic_error, "No more record in the RTD buffer!");
      snfee::io::rtd_record rec = _rtd_recs_.front();
      _rtd_recs_.pop_front();
      _front_trig_id_ = snfee::data::INVALID_TRIGGER_ID;
      _trig_ids_.clear();
      if (_rtd_recs_.size()) {
        // Update the current trigger ID with the ones from the next record:
        const snfee::io::rtd_record& next_rec = _rtd_recs_.front();
        _front_trig_id_ = next_rec.get_trigger_id();
        // Update the set of embedded trigger IDs:
        for (const auto& rtd_rec : _rtd_recs_) {
          _trig_ids_.insert(rtd_rec.get_trigger_id());
        }
      }
      return rec;
    }

    void
    rtd_buffer::terminate()
    {
      _terminated_ = true;
      return;
    }

    bool
    rtd_buffer::is_terminated() const
    {
      return _terminated_;
    }

    bool
    rtd_buffer::is_empty() const
    {
      return _rtd_recs_.size() == 0;
    }

    int32_t
    rtd_buffer::get_next_poppable_trig_id() const
    {
      if (is_empty())
        return snfee::data::INVALID_TRIGGER_ID;
      if (is_terminated())
        return _front_trig_id_;
      if (_trig_ids_.size() >= _min_popping_trig_ids_)
        return _front_trig_id_;
      return snfee::data::INVALID_TRIGGER_ID;
    }

    bool
    rtd_buffer::can_be_popped() const
    {
      if (is_empty())
        return false;
      if (is_terminated())
        return true;
      if (_trig_ids_.size() >= _min_popping_trig_ids_)
        return true;
      return false;
    }

    void
    rtd_buffer::print(std::ostream& out_) const
    {
      std::ostringstream out;
      out << "RTD buffer : " << std::endl;
      out << "|-- Minimum popping trig. IDs : " << _min_popping_trig_ids_
          << std::endl;
      out << "|-- Capacity : " << _capacity_ << std::endl;
      out << "|-- RTD records FIFO : " << _rtd_recs_.size() << std::endl;
      {
        std::size_t counter = 0;
        for (const auto& rtd_rec : _rtd_recs_) {
          out << "|   ";
          if (counter + 1 == _rtd_recs_.size()) {
            out << "`-- ";
          } else {
            out << "|-- ";
          }
          // out << "RHD";
          out << rtd_rec;
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
          if (counter == 0)
            print_it = true;
          if (counter + 1 == _trig_ids_.size())
            print_it = true;
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
      out << "|-- Terminated : " << std::boolalpha << _terminated_ << std::endl;
      out << "`-- Finished : " << std::boolalpha << is_finished() << std::endl;
      out << std::endl;
      out_ << out.str();
      return;
    }

  } // namespace redb
} // namespace snfee
