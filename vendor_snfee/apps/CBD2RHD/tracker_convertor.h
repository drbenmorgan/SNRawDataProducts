// Standard library:

#ifndef CBD2RHD_TRACKER_CONVERTOR_H
#define CBD2RHD_TRACKER_CONVERTOR_H

// This project:
#include <snfee/data/tracker_hit_record.h>

// SNComDAQ:
extern "C" {
  #include <sncomdaq/tracker_hit.h>
}

namespace cbd2rhd {
  
  void convert(const sncomdaq_tracker_hit & cbd_tracker_hit_,
	       snfee::data::tracker_hit_record & rhd_tracker_hit_);
  
}

#endif // CBD2RHD_TRACKER_CONVERTOR_H

