// Try and link in everything that will be in RTD
#include "snfee/data/trigger_record.h"
#include "snfee/data/tracker_hit_record.h"
#include "snfee/data/calo_hit_record.h"

// Has shared ptr, so not directly serializable
// #include "snfee/data/raw_trigger_data.h"
// Raw trigger data is
// - int32_t
// - int32_t
// - std::shared_ptr<const trigger_record>
// - std::vector<std::shared_ptr<calo_hit_record>>
// - std::vector<std::shared_ptr<tracker_hit_record>>

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
#pragma link C++ namespace snfee;
#pragma link C++ namespace snfee::data;

#pragma link C++ enum snfee::data::trigger_record::trigger_mode_type+;
#pragma link C++ class snfee::data::trigger_record+;

#pragma link C++ enum snfee::data::tracker_hit_record::channel_category_type+;
#pragma link C++ enum snfee::data::tracker_hit_record::timestamp_category_type+;
#pragma link C++ class snfee::data::tracker_hit_record+;

#pragma link C++ class snfee::data::calo_hit_record::two_channel_adc_record+;
#pragma link C++ class std::vector<snfee::data::calo_hit_record::two_channel_adc_record>+;
#pragma link C++ class snfee::data::calo_hit_record::waveforms_record+;
#pragma link C++ class snfee::data::calo_hit_record::channel_data_record+;
#pragma link C++ class snfee::data::calo_hit_record+;

