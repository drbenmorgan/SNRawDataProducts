// This project:
#include "RRawTriggerData.h"

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/utils.h>
#include <snfee/model/feb_constants.h>
#include <snfee/data/utils.h>

namespace snfee {
  namespace data {

    bool RRawTriggerData::isComplete() const
    {
      if (!hasRunID()) {
        return false;
      }
      if (!hasTriggerID()) {
        return false;
      }
      if (!trigger.is_complete()) {
        std::cerr << "[devel] trig issue !" << std::endl;
        return false;
      }
      if (caloRecords.size() + trackerRecords.size() == 0) {
        std::cerr << "[devel] hits issue !" << std::endl;
        return false;
      }
      for (const auto & chit : caloRecords) {
        if (!chit.is_complete()) {
          std::cerr << "[devel] calo hit issue !" << std::endl;
          return false;
        }
      }
      for (const auto & thit : trackerRecords) {
        if (!thit.is_complete()) {
          std::cerr << "[devel] tracker hit issue !" << std::endl;
          return false;
        }
      }
      return true;
    }

    bool RRawTriggerData::hasRunID() const
    {
      return runID != INVALID_RUN_ID;
    }

    int32_t RRawTriggerData::getRunID() const
    {
      return runID;
    }

    bool RRawTriggerData::hasTriggerID() const
    {
      return triggerID != INVALID_TRIGGER_ID;
    }

    int32_t RRawTriggerData::getTriggerID() const
    {
      return triggerID;
    }

    // virtual
    void RRawTriggerData::print_tree(std::ostream & out_,
                                      const boost::property_tree::ptree & options_) const
    {
      out_ << "NOTIMPLEMENTEDYET";
    }


    const TriggerRecord& RRawTriggerData::getTriggerRecord() const
    {
      return trigger;
    }

    const CaloRecordCollection& RRawTriggerData::getCaloRecords() const
    {
      return caloRecords;
    }

    const TrackerRecordCollection&
    RRawTriggerData::getTrackerRecords() const
    {
      return trackerRecords;
    }

  } // namespace data
} // namespace snfee
