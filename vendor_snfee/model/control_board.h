//! \file  snfee/model/control_board.h
//! \brief Description of the SuperNEMO front-end control board (CB) 
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

#ifndef SNFEE_MODEL_CONTROL_BOARD_H
#define SNFEE_MODEL_CONTROL_BOARD_H

// Standard library:
#include <string>

// Third party:
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/model/base_board.h>

namespace snfee {
  namespace model {

    /// \brief SuperNEMO control board
    class control_board
      : public base_board
    {
    public:
    
      enum cb_mode {
        MODE_UNDEF         = 0, ///< Undefined mode
        MODE_CALORIMETER   = 1, ///< Calorimeter mode
        MODE_TRACKER       = 2, ///< Tracker mode
        MODE_COMMISSIONING = 3  ///< Commissioning mode
      };

      static std::string cb_mode_label(const cb_mode);

      control_board();
 
      virtual ~control_board();

      bool has_mode() const;
    
      cb_mode get_mode() const;

      void set_mode(const cb_mode);

      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;
    
    private:

      virtual void _at_initialize_(const datatools::properties & config_) override;

      virtual void _at_reset_() override;

      void _construct_();

    private:

      // Configuration:
      cb_mode _mode_; ///!< Mode
    
    };

  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_CONTROL_BOARD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

