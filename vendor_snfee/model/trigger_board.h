//! \file  snfee/model/trigger_board.h
//! \brief Description of the SuperNEMO front-end trigger board (TB) 
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

#ifndef SNFEE_MODEL_TRIGGER_BOARD_H
#define SNFEE_MODEL_TRIGGER_BOARD_H

// Standard library:
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/model/base_board.h>

namespace snfee {
  namespace model {

    /// \brief SuperNEMO trigger board
    class trigger_board
      : public base_board
    {
    public:

      trigger_board();
   
      virtual ~trigger_board();
   
    private:

      virtual void _at_initialize_(const datatools::properties & config_) override;

      // virtual void _at_reset_() override;

      void _construct_();
 
    };
  
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_TRIGGER_BOARD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

