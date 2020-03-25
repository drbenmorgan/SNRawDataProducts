// snfee/io/raw_record_flow_statistics.cc

// Ourselves:
#include <snfee/io/raw_record_flow_statistics.h>

namespace snfee {
  namespace io {

    raw_record_flow_statistics::inversion_record::inversion_record(int pos1_,
                                                                   int pos2_,
                                                                   int tid1_,
                                                                   int tid2_)
    {
      pos1 = pos1_;
      pos2 = pos2_;
      tid1 = tid1_;
      tid2 = tid2_;
      return;
    }

    int
    raw_record_flow_statistics::inversion_record::get_dist() const
    {
      return pos2 - pos1;
    }

    raw_record_flow_statistics::raw_record_flow_statistics(
      const size_t buffer_size_,
      const bool debug_)
    {
      if (debug_)
        set_logging(datatools::logger::PRIO_DEBUG);
      _buffer_size_ = buffer_size_;
      _init_();
      return;
    }

    datatools::logger::priority
    raw_record_flow_statistics::get_logging() const
    {
      return _logging_;
    }

    void
    raw_record_flow_statistics::set_logging(
      const datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    int
    raw_record_flow_statistics::get_min_trigger_id()
    {
      return _first_positions_.begin()->first;
    }

    int
    raw_record_flow_statistics::get_max_inversion_distance()
    {
      return _max_inversion_distance_;
    }

    void
    raw_record_flow_statistics::_init_()
    {
      _hdistances_ = snfee::tools::histogram_int(0, 5000, 100);
      _max_inversion_distance_ = -1;
      return;
    }

    const snfee::tools::histogram_int&
    raw_record_flow_statistics::get_histogram_inversions() const
    {
      return _hdistances_;
    }

    void
    raw_record_flow_statistics::add(const int pos_, const int tid_)
    {
      if (!_first_positions_.count(tid_)) {
        // Record the position of the first occurence of a trigger ID:
        _first_positions_[tid_] = pos_;
      }
      // Compare with previous hits:
      bool detected_inversion = false;
      for (auto p : _first_positions_) {
        if (tid_ < p.first) {
          // Detected inversion:
          detected_inversion = true;
          int tid1 = p.first;
          int pos1 = p.second;
          int dist = pos_ - pos1;
          DT_LOG_DEBUG(_logging_,
                       "Detected inversion : trigger ID=" << tid_ << "@" << pos_
                                                          << " < " << tid1
                                                          << "@" << pos1);
          _inversions_[tid_] = inversion_record(pos1, pos_, tid1, tid_);
          _hdistances_.add(dist);
          if ((_max_inversion_distance_ < 0) ||
              (dist > _max_inversion_distance_)) {
            _max_inversion_distance_ = dist;
          }
          break;
        }
      }
      if (!detected_inversion) {
        while (tid_ > (get_min_trigger_id() + _buffer_size_)) {
          // Remove head:
          _first_positions_.erase(_first_positions_.begin());
        }
      }
      return;
    }

  } // namespace io
} // namespace snfee
