//! \file  snfee/model/base_firmware.h
//! \brief Description of the SuperNEMO front-end base firmware in a FPGA
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

#ifndef SNFEE_MODEL_BASE_FIRMWARE_H
#define SNFEE_MODEL_BASE_FIRMWARE_H

// Standard library:
#include <string>
#include <map>
#include <set>
#include <memory>
#include <iostream>

// Third party:
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/factory_macros.h>

// This project:
#include <snfee/model/utils.h>
#include <snfee/model/register_description.h>

namespace snfee {
  namespace model {
    class base_firmware;
  }
}

// Registration macros:

#define SNFEE_MODEL_FIRMWARE_REGISTRATION_INTERFACE(FIRMWARE_CLASS_NAME) \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::snfee::model::base_firmware,FIRMWARE_CLASS_NAME) \
  /**/

#define SNFEE_MODEL_FIRMWARE_REGISTRATION_IMPLEMENT(FIRMWARE_CLASS_NAME,FIRMWARE_ID) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::snfee::model::base_firmware,FIRMWARE_CLASS_NAME,FIRMWARE_ID) \
  /**/

namespace snfee {
  namespace model {

    /// \brief Base class for firmware objects
    class base_firmware
      : public datatools::i_tree_dumpable
    {
    public:

      base_firmware();

      virtual ~base_firmware();

      bool has_name() const;
    
      void set_name(const std::string &);

      const std::string & get_name() const;

      bool has_version() const;

      void set_version(const std::string &);

      const std::string & get_version() const;

      std::string full_id() const;

      bool has_category() const;
  
      void set_category(const firmware_category_type category_);

      firmware_category_type get_category() const;
    
      bool has_register(const std::string & name_) const;
    
      void add_register(const register_description & reg_, const std::string & name_ = "");

      void remove_register(const std::string & name_);

      const register_description & get_register(const std::string & name_) const;

      void build_register_names(std::set<std::string> &) const;

      bool has_register_at(const std::string & address_) const;
    
      /// Print the base_firmware structure
      ///
      /// Usage:
      /// \code
      /// snfee::model::base_firmware myFirmware
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Firmware");
      /// poptions.put("indent", ">>> ");
      /// myFirmware.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

      /// Check initialization flag
      bool is_initialized() const;

      /// Initialization from a set of configuration parameters
      void initialize(const datatools::properties & config_);

      /// Terminate
      void reset();

      /// Return the dictionary of register's descriptions
      const std::map<std::string, register_description> & get_registers() const;

      /// Return the names of the embedded registers
      std::set<std::string> get_register_names() const;
    
      /// Load the definitions of registers from an input stream
      void load_registers_from_csv(std::istream & in_);

      /// Load the definitions of registers from an input file
      void load_registers_from_csv_file(const std::string & filename_);

      /// Save the definitions of registers in an output stream
      void store_registers_to_csv(std::ostream & out_) const;

      /// Save the definitions of registers in an output file
      void store_registers_to_csv_file(const std::string & filename_) const;

      bool is_register_layout_locked() const;

      void lock_register_layout();

      void unlock_register_layout();
    
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
      std::string _name_;    ///< Name of the firmware
      std::string _version_; ///< Name of the version
      firmware_category_type _category_; ///< Name of the version

      // Usage:
      bool _register_layout_locked_ = false;
      std::map<std::string, register_description> _registers_; ///! List of register descriptions

      // Factory interface :
      DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_firmware)

      // Registration interface :
      SNFEE_MODEL_FIRMWARE_REGISTRATION_INTERFACE(base_firmware)

    };
 
    typedef std::shared_ptr<base_firmware> firmware_ptr;
  
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_BASE_FIRMWARE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
