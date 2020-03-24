// -*- mode: c++ ; -*-
/// \file snfee/data/raw_event_data-serial.h

#ifndef SNFEE_DATA_RAW_EVENT_DATA_SERIAL_H
#define SNFEE_DATA_RAW_EVENT_DATA_SERIAL_H 1

// Ourselves:
#include <snfee/data/raw_event_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <snfee/data/time-serial.h>

namespace snfee {
  namespace data {

    /// Serialization method
    template <class Archive>
    void raw_event_data::rtd_record_type::serialize(Archive & ar_,
                                                    const unsigned int /* version */)
    {
      ar_ & boost::serialization::make_nvp("time_shift", time_shift);
      ar_ & boost::serialization::make_nvp("rtd",       rtd);
      return;
    }

    /// Serialization method
    template <class Archive>
    void raw_event_data::serialize(Archive & ar_,
                                   const unsigned int /* version */)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp("run_id",   _run_id_);
      ar_ & boost::serialization::make_nvp("event_id", _event_id_);
      ar_ & boost::serialization::make_nvp("reference_time", _reference_time_);
      ar_ & boost::serialization::make_nvp("rtd_pack", _rtd_pack_);
      return;
    }

  } // end of namespace data
} // end of namespace snfee

#endif  // SNFEE_DATA_RAW_EVENT_DATA_SERIAL_H
