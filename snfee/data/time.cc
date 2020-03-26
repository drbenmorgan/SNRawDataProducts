// This project:
#include <snfee/data/time.h>

namespace snfee {
  namespace data {

    std::string
    clock_label(const clock_type clk_)
    {
      if (clk_ == CLOCK_40MHz) {
        return std::string("40MHz");
      }
      if (clk_ == CLOCK_80MHz) {
        return std::string("80MHz");
      }
      if (clk_ == CLOCK_160MHz) {
        return std::string("160MHz");
      }
      if (clk_ == CLOCK_2560MHz) {
        return std::string("2560MHz");
      }
      return std::string("");
    }

    uint32_t
    clock_comparison_factor(const clock_type clk_)
    {
      if (clk_ == CLOCK_40MHz) {
        return 64;
      }
      if (clk_ == CLOCK_80MHz) {
        return 32;
      }
      if (clk_ == CLOCK_160MHz) {
        return 16;
      }
      if (clk_ == CLOCK_2560MHz) {
        return 1;
      }
      return 0;
    }

    // static
    int
    timestamp::compare(const timestamp& ts1_,
                       const timestamp& ts2_,
                       bool strict_)
    {
      if (ts1_._clock_ == CLOCK_UNDEF) {
        return -2;
      }
      if (ts2_._clock_ == CLOCK_UNDEF) {
        return -2;
      }
      if (strict_) {
        if (ts1_._clock_ != ts2_._clock_) {
          // Compare only timestamps using the same clock:
          return -2;
        }
        if (ts1_._ticks_ < ts2_._ticks_) {
          return -1;
        }
        if (ts1_._ticks_ > ts2_._ticks_) {
          return +1;
        }
        return 0;
      }
      int f1 = clock_comparison_factor(ts1_._clock_);
      int f2 = clock_comparison_factor(ts2_._clock_);
      int64_t t1 = f1 * ts1_._ticks_;
      int64_t t2 = f2 * ts2_._ticks_;
      if (t1 < t2) {
        return -1;
      }
      if (t1 > t2) {
        return +1;
      }
      if (f1 > f2) {
          return -1;
      }
      return 0;
    }

    timestamp::timestamp(const int64_t ticks_)
      : _clock_(CLOCK_40MHz), _ticks_(ticks_)
    {}

    timestamp::timestamp(const clock_type clock_, const int64_t ticks_)
      : _clock_(clock_), _ticks_(ticks_)
    {}

    void
    timestamp::invalidate()
    {
      _clock_ = CLOCK_UNDEF;
      _ticks_ = INVALID_TICKS;
    }

    bool
    timestamp::is_valid() const
    {
      if (_clock_ == CLOCK_UNDEF) {
        return false;
      }
      if (_ticks_ == INVALID_TICKS) {
        return false;
      }
      return true;
    }

    void
    timestamp::set_clock(const clock_type clock_)
    {
      _clock_ = clock_;
    }

    clock_type
    timestamp::get_clock() const
    {
      return _clock_;
    }

    bool
    timestamp::is_clock_40MHz() const
    {
      return _clock_ == CLOCK_40MHz;
    }

    bool
    timestamp::is_clock_80MHz() const
    {
      return _clock_ == CLOCK_80MHz;
    }

    bool
    timestamp::is_clock_160MHz() const
    {
      return _clock_ == CLOCK_160MHz;
    }

    bool
    timestamp::is_clock_2560MHz() const
    {
      return _clock_ == CLOCK_2560MHz;
    }

    void
    timestamp::set_ticks(const int64_t ticks_)
    {
      _ticks_ = ticks_;
    }

    int64_t
    timestamp::get_ticks() const
    {
      return _ticks_;
    }

    std::ostream&
    operator<<(std::ostream& out_, const timestamp& ts_)
    {
      if (ts_.is_valid()) {
        out_ << "[clock=" << clock_label(ts_._clock_)
             << ";ticks=" << ts_._ticks_ << "]";
      } else {
        out_ << "[invalid]";
      }
      return out_;
    }

    bool
    timestamp::operator<(const timestamp& other_) const
    {
      return compare(*this, other_) == -1;
    }

  } // namespace data
} // namespace snfee
