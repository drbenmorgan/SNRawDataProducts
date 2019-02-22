// -*- mode: c++; -*-
/// \file snfee/the_serializable.h

#ifndef SNFEE_THE_SERIALIZABLE_H
#define SNFEE_THE_SERIALIZABLE_H 1

// Third party:
// - Boost:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <boost/serialization/export.hpp>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// - Bayeux/datatools:
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <datatools/archives_instantiation.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <snfee/data/calo_hit_record-serial.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::calo_hit_record)
BOOST_CLASS_EXPORT_IMPLEMENT(snfee::data::calo_hit_record)

#include <snfee/data/tracker_hit_record-serial.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::tracker_hit_record)
BOOST_CLASS_EXPORT_IMPLEMENT(snfee::data::tracker_hit_record)

#include <snfee/data/trigger_record-serial.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::trigger_record)
BOOST_CLASS_EXPORT_IMPLEMENT(snfee::data::trigger_record)

#include <snfee/data/raw_trigger_data-serial.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::raw_trigger_data)
BOOST_CLASS_EXPORT_IMPLEMENT(snfee::data::raw_trigger_data)

#include <snfee/data/run_info-serial.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::run_info)
BOOST_CLASS_EXPORT_IMPLEMENT(snfee::data::run_info)

#include <snfee/data/channel_id-serial.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::channel_id)

#include <snfee/data/calo_waveform_data-serial.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::calo_waveform)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::calo_waveform_baseline)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::calo_waveform_peak)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::calo_waveform_cfd)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::calo_waveform_charge)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::calo_waveform_firmware_data)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::calo_waveform_info)

#include <snfee/data/calo_channel_traits-serial.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(snfee::data::calo_channel_traits)

namespace snfee {

}  // end of namespace snfee

#endif // SNFEE_THE_SERIALIZABLE_H

