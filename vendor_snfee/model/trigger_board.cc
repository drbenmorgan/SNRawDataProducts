// This project:
#include <snfee/model/trigger_board.h>

// Third party:
// - Bayeux:
#include <bayeux/datatools/exception.h>

// This project:
#include <snfee/model/utils.h>

namespace snfee {
  namespace model {
  
    trigger_board::trigger_board()
    {
      return;
    }

    // virtual
    trigger_board::~trigger_board()
    {
      return;
    }

    void trigger_board::_construct_()
    {
      set_model(BOARD_TB);
      add_fpga(FPGA_GENERIC);
      lock_fpga_layout();
      return;
    }

    // virtual
    void trigger_board::_at_initialize_(const datatools::properties & config_)
    {
      _construct_();
      base_board::_common_initialize(config_);
      return;
    }

    // // virtual
    // void trigger_board::_at_reset_()
    // {
    //   unlock_fpga_layout();
    //   base_board::_common_reset();
    //   return;
    // }

  } // namespace model
} // namespace snfee
