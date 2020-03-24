//! \file  snfee/model/crate.h
//! \brief Description of the SuperNEMO front-end crate 
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

#ifndef SNFEE_MODEL_CRATE_H
#define SNFEE_MODEL_CRATE_H

// Standard library:
#include <string>
#include <memory>

// Third party:
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/model/base_board.h>
#include <snfee/model/board_builder.h>

namespace snfee {
  namespace model {
 
    /// \brief SuperNEMO front-end readout crate 
    class crate
      : public datatools::i_tree_dumpable
    {
    public:
    
      static const int MAX_SLOT           = 20;
      static const int CONTROL_BOARD_SLOT = 10;
      static const int TRIGGER_BOARD_SLOT = 20;

      static std::string build_crate_name(const crate_model_type model_,
                                          const int id_);
     
      crate();

      virtual ~crate();

      bool has_name() const;
    
      void set_name(const std::string &);

      const std::string & get_name() const;

      bool has_model() const;

      void set_model(const crate_model_type);

      crate_model_type get_model() const;
    
      bool is_calorimeter_model() const;

      bool is_tracker_model() const;

      bool is_commissioning_model() const;

      bool has_id() const;
    
      void set_id(const int);

      int get_id() const;

      bool has_board(const std::string & name_) const;
 
      bool has_board_at_slot(const int slot_) const;
    
      void add_board(const board_ptr & board_, const int slot_);
    
      void remove_board(const int slot_);

      const base_board & get_board(const int slot_) const;

      const base_board & get_board(const std::string & name_) const;

      base_board & grab_board(const int slot_);

      base_board & grab_board(const std::string & name_);

      std::set<std::string> get_board_names() const;

      /// Change the firmware code of an embedded FPGA in a board
      void flash_fpga(const firmware_builder & fwb_,
                      const std::string & fwid_,
                      const int slot_,
                      const std::string & fpga_name_);

      /// Change the firmware code of an embedded FPGA in a board
      void flash_fpga(const firmware_builder & fwb_,
                      const std::string & fwid_,
                      const std::string & board_name_,
                      const std::string & fpga_name_);
     
      /// Print the crate structure
      ///
      /// Usage:
      /// \code
      /// snfesetup::crate myCrate
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Crate");
      /// poptions.put("indent", ">>> ");
      /// myCrate.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;
 
      /// Check initialization flag
      bool is_initialized() const;

      /// Standalone initialization 
      void initialize_simple();

      /// Initialization from a set of configuration parameters
      void initialize(const datatools::properties & config_,
                      const board_builder * bb_ = nullptr);

      /// Terminate
      void reset();

      bool is_board_layout_locked() const;

      void lock_board_layout();

      void unlock_board_layout();
    
    private:

      // Management:
      bool _initialized_ = false;

      // Configuration:
      std::string       _name_;
      crate_model_type  _model_ = CRATE_UNDEF;
      int               _id_ = -1;

      // Usage:
      bool _board_layout_locked_ = false;
      std::map<int, board_ptr> _boards_;

    };
  
    typedef std::shared_ptr<crate> crate_ptr;
 
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_CRATE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
