//! \file  snfesetup/tb_firmware.h
//! \brief Description of the SuperNEMO Trigger Board (TB) firmware
//
// Copyright (c) 2018 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of SNFrontEndElectronics.
//
// SNFrontEndElectronics is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SNFrontEndElectronics is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SNFrontEndElectronics. If not, see <http://www.gnu.org/licenses/>.

#ifndef SNFESETUP_TB_FIRMWARE_H
#define SNFESETUP_TB_FIRMWARE_H

// This project:
#include <snfesetup/base_firmware.h>

namespace snfesetup {

  /// \brief Specialized firmware for the Trigger Board
  class tb_firmware
    : public base_firmware
  {
  public:

    tb_firmware();

    virtual ~tb_firmware();

  private:

    virtual void _at_initialize_(const datatools::properties & config_) override;

    virtual void _at_reset_() override;

    void _construct_(const datatools::properties & config_);
    
    void _destroy_();
   
  };
   
} // namespace snfesetup

#endif // SNFESETUP_TB_FIRMWARE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
