// This project:

#include "trigger_convertor.h"

namespace cbd2rhd {
  
  void convert(const sncomdaq_trigger_hit & cbd_trigger_hit_,
               snfee::data::trigger_record & rhd_trigger_hit_)
  {
    rhd_trigger_hit_.invalidate();

    // Trigger record mode:
    snfee::data::trigger_record::trigger_mode_type trigger_mode
      = snfee::data::trigger_record::TRIGGER_MODE_INVALID;
    switch (cbd_trigger_hit_.trigger_mode) {
    case SNCOMDAQ_TRIGGER_CALO_ONLY:
      trigger_mode = snfee::data::trigger_record::TRIGGER_MODE_CALO_ONLY;
      break;
    case SNCOMDAQ_TRIGGER_CARACO:
      trigger_mode = snfee::data::trigger_record::TRIGGER_MODE_CARACO;
      break;
    case SNCOMDAQ_TRIGGER_APE:
      trigger_mode = snfee::data::trigger_record::TRIGGER_MODE_APE;
      break;
    case SNCOMDAQ_TRIGGER_DAVE:
      trigger_mode = snfee::data::trigger_record::TRIGGER_MODE_DAVE;
      break;
    default:
      break;
    }
    
    
    int32_t hit_num = 0;
    rhd_trigger_hit_.make(hit_num,
                          cbd_trigger_hit_.trigger_id,
                          trigger_mode);
    return;
  }
  
}
