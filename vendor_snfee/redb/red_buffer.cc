// This project:
#include <snfee/redb/red_buffer.h>

namespace snfee {
  namespace redb {

    red_buffer::red_buffer()
    {
      return;
    }

    red_buffer::~red_buffer()
    {
      _front_event_id_ = snfee::data::INVALID_EVENT_ID;
      _red_recs_.clear();
      return;
    }

    void red_buffer::terminate()
    {
      _terminated_ = true;
      return;
    }
      
    bool red_buffer::is_terminated()
    {
      return _terminated_;
    }
      
    bool red_buffer::is_empty()
    {
      return _red_recs_.empty();
    }

    void red_buffer::push_record(const snfee::io::red_record & rec_)
    {
      // Check ordering ???
      int32_t new_event_id = rec_.get_event_id();
      _front_event_id_ = new_event_id; 
      _red_recs_.push_back(rec_);
      return;
    }
  
    snfee::io::red_record red_buffer::pop_record()
    {
      snfee::io::red_record rec = _red_recs_.front();
      _red_recs_.pop_front();
      _front_event_id_ = snfee::data::INVALID_EVENT_ID;
      if (_red_recs_.size()) {
        // Update the current event ID with the ones from the next record in the FIFO:
        const snfee::io::red_record & next_rec = _red_recs_.front();
        _front_event_id_ = next_rec.get_event_id();
      }
      return rec;
    }

    void red_buffer::print(std::ostream & out_) const
    {
      std::ostringstream out;
      out << "RED buffer: " << std::endl;
      out << "|-- RED records FIFO : " << _red_recs_.size() << std::endl;
      std::size_t counter = 0;
      for (const auto & red_rec : _red_recs_) {
        out << "|   ";
        if (counter + 1 == _red_recs_.size()) {
          out << "`-- ";
        } else  {
          out << "|-- ";
        }
        out << red_rec;
        out << std::endl;
        counter++;
      }
      out << "|-- Front event ID : " << _front_event_id_ << std::endl;
      out << "`-- Terminated       : " << std::boolalpha << _terminated_ << std::endl;
      out << std::endl;
      out_ << out.str();
      return;
    }
   
  } // namespace redb
} // namespace snfee
