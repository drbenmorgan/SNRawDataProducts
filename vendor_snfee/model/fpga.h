//! \file  snfee/model/fpga.h
//! \brief Description of the SuperNEMO front-end fpga 
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

#ifndef SNFEE_MODEL_FPGA_H
#define SNFEE_MODEL_FPGA_H

// Standard library:
#include <string>
#include <memory>

// Third party:
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/properties.h>

// This project:
#include <snfee/model/utils.h>
#include <snfee/model/base_firmware.h>

namespace snfee {
  namespace model {

    class firmware_builder;

    /// \brief Description of a FPGA chip
    class fpga
      : public datatools::i_tree_dumpable
    {
    public:

      static std::string build_fpga_name(const fpga_model_type model_,
                                         const int id_ = -1);

      fpga();

      virtual ~fpga();
    
      bool has_name() const;
    
      void set_name(const std::string &);

      const std::string & get_name() const;

      bool has_model() const;

      void set_model(const fpga_model_type);

      bool is_control_model() const;

      bool is_frontend_model() const;

      bool is_generic_model() const;

      bool has_id() const;
 
      void set_id(const int id_);

      int get_id() const;

      bool has_code() const;

      void set_code(const firmware_ptr & code_);

      const base_firmware & get_code() const;

      void flash(const firmware_builder & fwb_, const std::string & fwid_);
    
      /// Print the FPGA structure
      ///
      /// Usage:
      /// \code
      /// snfesetup::fpga myFPGA
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "FPGA");
      /// poptions.put("indent", ">>> ");
      /// myFPGA.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

      /// Check initialization flag
      bool is_initialized() const;

      /// Standalone initialization 
      void initialize_simple();

      /// Initialization from a set of configuration parameters
      void initialize(const datatools::properties & config_,
                      firmware_builder * fb_ = nullptr);

      /// Terminate
      void reset();

      bool is_code_locked() const;

      void lock_code();

      void unlock_code();

    private:

      // Management:
      bool _initialized_ = false;
    
      // Configuration:
      std::string     _name_;
      fpga_model_type _model_ = FPGA_UNDEF;
      int             _id_ = -1;

      // Usage:
      bool _code_lock_ = false;
      firmware_ptr _code_; ///< Identifier of the loaded firmware code
    
    };

    typedef std::shared_ptr<fpga> fpga_ptr;
  
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_FPGA_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
