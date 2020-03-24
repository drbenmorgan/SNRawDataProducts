//! \file  snfee/model/base_board.h
//! \brief Description of the SuperNEMO base front-end board 
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

#ifndef SNFEE_MODEL_BASE_BOARD_H
#define SNFEE_MODEL_BASE_BOARD_H

// Standard library:
#include <string>
#include <map>
#include <set>
#include <memory>

// Third party:
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>

// This project:
#include <snfee/utils.h>
#include <snfee/model/utils.h>
#include <snfee/model/fpga.h>
#include <snfee/model/fpga_builder.h>

namespace snfee {
  namespace model {

    class firmware_builder;

    /// \brief SuperNEMO base front-end readout board 
    class base_board
      : public datatools::i_tree_dumpable
    {
    public:

      static std::string build_board_name(const board_model_type model_, const int id_ = -1);

      base_board();

      virtual ~base_board();
 
      bool has_name() const;
    
      void set_name(const std::string &);

      const std::string & get_name() const;

      bool has_model() const;
    
      void set_model(const board_model_type);

      board_model_type get_model() const;

      /// Add a fully initialized FPGA in the board
      void add_fpga(const fpga & pga_);

      /// Add a FPGA in the board without firmware code
      void add_fpga(const fpga_model_type model_, const int id_ = -1);

      /// Remove a FPGA from the board
      void remove_fpga(const std::string & name_);

      /// Remove a FPGA
      void remove_fpga(const fpga_model_type model_, const int id_ = -1);

      /// Check if a FPGA with given name exists
      bool has_fpga(const std::string & name_) const;

      /// Check if a FPGA with given model and index exists
      bool has_fpga(const fpga_model_type model_, const int id_ = -1) const;

      /// Return FPGA by name
      const fpga & get_fpga(const std::string &) const;
  
      /// Return FPGA by model and index
      const fpga & get_fpga(const fpga_model_type model_, const int id_ = -1) const;

      /// Change the firmware code of an embedded FPGA
      void flash_fpga(const firmware_builder & fwb_,
                      const std::string & fwid_,
                      const std::string & name_);

      /// Change the firmware code of an embedded FPGA
      void flash_fpga(const firmware_builder & fwb_,
                      const std::string & fwid_,
                      const fpga_model_type model_,
                      const int id_ = -1);
   
      /// Check if the board has an absolute ID
      bool has_id() const;
    
      /// Set the ID
      void set_id(const int);

      /// Return the ID
      int get_id() const;
    
      /// Check if the board is plugged in a slot
      bool has_slot() const;
    
      /// Set the slot number where the board is plugged
      void set_slot(const int);

      /// Return the slot
      int get_slot() const;

      /// Check if the board has an address
      bool has_address() const;

      /// Set the board's address
      void set_address(const std::string &);

      /// Return the board address
      const std::string & get_address() const;

      /// Return the names of the embedded FPGAs
      std::set<std::string> get_fpga_names() const;

      /// Check if all embedded FPGAs have firmware code
      bool all_fpgas_have_code() const;

      /// Check if all embedded FPGAs have firmware code
      bool all_fpgas_are_locked() const;

      /// Lock the firmware code of all embedded FPGAs
      void lock_all_fpgas_code();

      /// Unlock the firmware code of all embedded FPGAs
      void unlock_all_fpgas_code();
    
      /// Print the board structure
      ///
      /// Usage:
      /// \code
      /// snfesetup::base_board myBoard
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Board");
      /// poptions.put("indent", ">>> ");
      /// myBoard.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

      /// Check initialization flag
      bool is_initialized() const;

      /// Initialization from a set of configuration parameters
      void initialize(const datatools::properties & config_);

      /// Standalone initialization
      void initialize_simple();

      /// Terminate
      void reset();

      bool is_fpga_layout_locked() const;

      void lock_fpga_layout();

      void unlock_fpga_layout();

    protected:

      void _common_initialize(const datatools::properties & config_);

      void _common_reset();
    
    private:

      virtual void _at_initialize_(const datatools::properties & config_);

      virtual void _at_reset_();
    
    private:

      // Management:
      bool _initialized_ = false; ///< Initialization flag

      // Configuration:
      std::string _name_;                  ///< Name of the board
      board_model_type _model_ = BOARD_UNDEF;    ///< Board model
      int         _id_   = -1;             ///< Board ID 
      int         _slot_ = -1;             ///< Board slot position (if plugged in a crate)
      std::string _address_;               ///< Address of the board
      bool        _fpga_layout_locked_ = false;
      std::map<std::string, fpga> _fpgas_; ///< Dictionary of FPGA chips

    };
 
    typedef std::shared_ptr<base_board> board_ptr;
  
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_BASE_BOARD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
