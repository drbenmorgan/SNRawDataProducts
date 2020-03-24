// Ourselves:
#include <snfesetup/tb_firmware.h>

// This project:
// #include <snfesetup/utils.h>


namespace snfesetup {

  tb_firmware::tb_firmware()
  {
    return;
  }

  // virtual
  tb_firmware::~tb_firmware()
  {
    return;
  }
  
  // virtual
  void tb_firmware::_at_initialize_(const datatools::properties & config_)
  {
    _common_initialize(config_);
    _construct_(config_);
    return;
  }

  // virtual
  void tb_firmware::_at_reset_()
  {
    _common_reset();
    return;
  }

  // virtual
  void tb_firmware::_construct_(const datatools::properties & config_)
  {
    return;
  }

  // virtual
  void tb_firmware::_destroy_()
  {
    return;
  }

} // namespace snfesetup
