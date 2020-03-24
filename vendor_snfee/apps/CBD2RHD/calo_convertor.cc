// This project:

#include "calo_convertor.h"

namespace cbd2rhd {
  
  void convert(const sncomdaq_calorimeter_hit & cbd_calo_hit_,
               snfee::data::calo_hit_record & rhd_calo_hit_)
  {
    rhd_calo_hit_.invalidate();

    bool has_waveforms = (cbd_calo_hit_.waveform_datasize > 0);
    uint16_t waveform_start_sample = 0;
    uint16_t waveform_number_of_samples = cbd_calo_hit_.waveform_datasize;
    rhd_calo_hit_.make(cbd_calo_hit_.hit_number,
                       cbd_calo_hit_.trigger_id,
                       cbd_calo_hit_.tdc,
                       cbd_calo_hit_.crate_id,
                       cbd_calo_hit_.board_id,
                       cbd_calo_hit_.chip_id,
                       cbd_calo_hit_.event_id,
                       cbd_calo_hit_.l2_id,
                       cbd_calo_hit_.fcr,
                       has_waveforms,
                       waveform_start_sample,
                       waveform_number_of_samples,
                       false
                       );

    for (unsigned int samlong_channel = 0;
         samlong_channel < snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS;
         samlong_channel++) {

      // Compute the position of the rising/falling edges, corrected by an FPGA-interpolated offset:
      int32_t rising_cell =
        cbd_calo_hit_.cfd_rising_edge_cell[samlong_channel] * 256 
        + cbd_calo_hit_.cfd_rising_edge_offset[samlong_channel];
      int32_t falling_cell =
        cbd_calo_hit_.cfd_falling_edge_cell[samlong_channel] * 256 
        + cbd_calo_hit_.cfd_falling_edge_offset[samlong_channel];
      
      rhd_calo_hit_.make_channel(samlong_channel,
                                 cbd_calo_hit_.lt[samlong_channel] > 0 ? true : false,
                                 cbd_calo_hit_.ht[samlong_channel] > 0 ? true : false,
                                 cbd_calo_hit_.underflow[samlong_channel] > 0 ? true : false,
                                 cbd_calo_hit_.overflow[samlong_channel] > 0 ? true : false,
                                 cbd_calo_hit_.baseline[samlong_channel],
                                 cbd_calo_hit_.peak[samlong_channel],
                                 cbd_calo_hit_.peak_cell[samlong_channel],
                                 cbd_calo_hit_.charge[samlong_channel],
                                 rising_cell,
                                 falling_cell
                                 );
      
      rhd_calo_hit_.make_channel_trigger_stat(samlong_channel,
                                              cbd_calo_hit_.low_threshold_trig_count[samlong_channel],
                                              cbd_calo_hit_.low_threshold_time_count[samlong_channel]
                                              );
    }
    
    if (has_waveforms) {
      for (unsigned int samlong_channel = 0;
           samlong_channel < snfee::model::feb_constants::SAMLONG_NUMBER_OF_CHANNELS;
           samlong_channel++) {
        for (int isample = 0; isample < cbd_calo_hit_.waveform_datasize; isample++) {
          rhd_calo_hit_.set_waveform_adc(samlong_channel,
                                         isample,
                                         cbd_calo_hit_.waveform_data[samlong_channel][isample]);
        }
      }
    }
    
    return;
  }
  
}
