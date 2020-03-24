// This project:
#include <snfee/model/tracker_fe_board.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/utils.h>
#include <snfee/model/feb_constants.h>

namespace snfee {
  namespace model {

    tracker_fe_board::tracker_fe_board()
    {
      return;
    }

    // virtual
    tracker_fe_board::~tracker_fe_board()
    {
      return;
    }

    void tracker_fe_board::_construct_()
    {
      set_model(BOARD_TFEB);
      add_fpga(FPGA_CONTROL);
      for (int i = 0; i < feb_constants::TFEB_NUMBER_OF_FE_FPGA; i++) {
        add_fpga(FPGA_FRONTEND, i);
      }
      lock_fpga_layout();
      return;
    }
  
    // virtual
    void tracker_fe_board::_at_initialize_(const datatools::properties & config_)
    {
      _construct_();
      base_board::_common_initialize(config_);
      return;
    }

  } // namespace model
} // namespace snfee
