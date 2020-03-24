// This project:

#include "tracker_convertor.h"

namespace cbd2rhd {
  
  void convert(const sncomdaq_tracker_hit & cbd_tracker_hit_,
               snfee::data::tracker_hit_record & rhd_tracker_hit_)
  {
    rhd_tracker_hit_.invalidate();

    // Hit channel category:
    snfee::data::tracker_hit_record::channel_category_type channel_category
      = snfee::data::tracker_hit_record::CHANNEL_UNDEF;
    switch (cbd_tracker_hit_.channel_category) {
    case SNCOMDAQ_TRKCH_ANODE :
      channel_category = snfee::data::tracker_hit_record::CHANNEL_ANODE;
      break;
    case SNCOMDAQ_TRKCH_CATHODE :
      channel_category = snfee::data::tracker_hit_record::CHANNEL_CATHODE;
      break;
    default:
      break;
    };

    // Hit timestamp category:
    snfee::data::tracker_hit_record::timestamp_category_type timestamp_category
      = snfee::data::tracker_hit_record::TIMESTAMP_UNDEF;
    switch (cbd_tracker_hit_.timestamp_category) {
    case SNCOMDAQ_TRKTS_ANODE_R0 :
      timestamp_category = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R0;
      break;
    case SNCOMDAQ_TRKTS_ANODE_R1 :
      timestamp_category = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R1;
      break;
    case SNCOMDAQ_TRKTS_ANODE_R2 :
      timestamp_category = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R2;
      break;
    case SNCOMDAQ_TRKTS_ANODE_R3 :
      timestamp_category = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R3;
      break;
    case SNCOMDAQ_TRKTS_ANODE_R4 :
      timestamp_category = snfee::data::tracker_hit_record::TIMESTAMP_ANODE_R4;
      break;
    case SNCOMDAQ_TRKTS_CATHODE_R5 :
      timestamp_category = snfee::data::tracker_hit_record::TIMESTAMP_CATHODE_R5;
      break;
    case SNCOMDAQ_TRKTS_CATHODE_R6 :
      timestamp_category = snfee::data::tracker_hit_record::TIMESTAMP_CATHODE_R6;
      break;
    default:
      break;
    };
   
    rhd_tracker_hit_.make(cbd_tracker_hit_.hit_number,
                          cbd_tracker_hit_.trigger_id,
                          cbd_tracker_hit_.crate_id,
                          cbd_tracker_hit_.board_id,
                          cbd_tracker_hit_.chip_id,
                          cbd_tracker_hit_.channel_id,
                          channel_category,
                          timestamp_category,
                          cbd_tracker_hit_.timestamp);
    
    return;
  }
  
}
