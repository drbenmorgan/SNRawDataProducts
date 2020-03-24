// Standard library:

#ifndef CBD2RHD_CALO_CONVERTOR_H
#define CBD2RHD_CALO_CONVERTOR_H

// This project:
#include <snfee/data/calo_hit_record.h>

// SNComDAQ:
extern "C" {
  #include <sncomdaq/calorimeter_hit.h>
}

namespace cbd2rhd {
  
  void convert(const sncomdaq_calorimeter_hit & cbd_calo_hit_,
	       snfee::data::calo_hit_record & rhd_calo_hit_);
  
}

#endif // CBD2RHD_CALO_CONVERTOR_H

