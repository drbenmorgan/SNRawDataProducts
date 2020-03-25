// -*- mode: c++ ; -*-
/// \file snfee/data/time-serial.h

#ifndef SNFEE_DATA_TIME_SERIAL_H
#define SNFEE_DATA_TIME_SERIAL_H 1

// Ourselves:
#include <snfee/data/time.h>

// Third party:
// - Boost:
#include <boost/serialization/nvp.hpp>
/* #include <boost/date_time/gregorian/greg_serialize.hpp> */
/* #include <boost/date_time/posix_time/time_serialize.hpp> */

namespace snfee {
  namespace data {

    /// Serialization method
    template <class Archive>
    void
    timestamp::serialize(Archive& ar_, const unsigned int /* version */)
    {
      ar_& boost::serialization::make_nvp("clock", _clock_);
      ar_& boost::serialization::make_nvp("ticks", _ticks_);
      return;
    }

  } // end of namespace data
} // end of namespace snfee

#endif // SNFEE_DATA_TIME_SERIAL_H
