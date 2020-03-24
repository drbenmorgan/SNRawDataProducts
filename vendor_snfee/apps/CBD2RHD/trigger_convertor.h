// Standard library:

#ifndef CBD2RHD_TRIGGER_CONVERTOR_H
#define CBD2RHD_TRIGGER_CONVERTOR_H

// This project:
#include <snfee/data/trigger_record.h>

// SNComDAQ:
extern "C" {
  #include <sncomdaq/trigger_hit.h>
}

namespace cbd2rhd {
  
  void convert(const sncomdaq_trigger_hit & cbd_trigger_hit_,
	       snfee::data::trigger_record & rhd_trigger_hit_);
  
}

#endif // CBD2RHD_TRIGGER_CONVERTOR_H

