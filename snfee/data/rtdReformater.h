//! \file snfee/data/rtdReformater.h
//! \brief Converters between "online/offline" RTD records
#ifndef RTDREFORMATER_HH
#define RTDREFORMATER_HH

#include <snfee/data/raw_trigger_data.h>
#include <snfee/data/RRawTriggerData.h>

namespace snfee {
namespace data {

//! Convert input "online" RTD record to "offline" format
snfee::data::RRawTriggerData
rtdOnlineToOffline(const snfee::data::raw_trigger_data& rawRTD);

} // namespace data
} // namespace snfee

#endif // RTDREFORMATER_HH

