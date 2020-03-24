//! \file  snfee/model/firmware_builder.h
//! \brief Special builder for the instantiation of registered SuperNEMO firmware models 
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

#ifndef SNFEE_MODEL_FIRMWARE_BUILDER_H
#define SNFEE_MODEL_FIRMWARE_BUILDER_H

// Standard library:
#include <string>
#include <set>
#include <map>

// Third party:
// - Bayeux:     
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/logger.h>

// This project:
#include <snfee/model/utils.h>
#include <snfee/model/base_firmware.h>

namespace snfee {
  namespace model {

    /// \brief SuperNEMO front-end electronics firmware builder 
    class firmware_builder
      : public datatools::i_tree_dumpable
    {
    public:

      /// \brief Firmware registration entry
      struct firmware_db_entry {
        std::string            id;      ///< Unique ID of the firmware (name & version)
        std::string            type;    ///< ID of the registered firmware type (registered class)
        firmware_category_type cat;     ///< Firmware category
        datatools::properties  config;  ///< Configuration parameters
      };
    
      typedef std::map<std::string, firmware_db_entry> firmware_db_type;
      typedef std::map<std::string, firmware_ptr> firmware_pool_type;
    
      /// Default constructor
      firmware_builder();
    
      /// Destructor
      virtual ~firmware_builder();

      /// Check is a firmware category is supported
      bool is_firmware_category_supported(const firmware_category_type category_) const;

      /// Add a supported firmware category
      void add_supported_firmware_category(const firmware_category_type category_);

      /// Return the set of supported firmware types 
      const std::set<firmware_category_type> & get_supported_firmware_categories() const;

      /// Add a path to files for definition of registered firmwares
      void add_firmware_db_path(const std::string & path_);

      /// Return the paths to files for definition of registered firmwares
      const std::set<std::string> & get_firmware_db_paths() const;
    
      /// Check initialization flag
      bool is_initialized() const;

      /// Initialization from a set of configuration parameters
      void initialize(const datatools::properties & config_);

      /// Initialization froms scratch
      void initialize_simple();

      /// Terminate
      void reset();

      /// Smart print
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

      /// Check if a firmware is registered
      bool has_registered_firmware(const std::string & fwid_) const;

      /// Check if a firmware is instantiated
      bool has_firmware(const std::string & fwid_) const;

      /// Return a firmware instance
      firmware_ptr get_firmware(const std::string & fwid_) const;
      
      /// Firmware factory method
      virtual firmware_ptr make_firmware(const std::string & fwid_,
                                         const firmware_category_type fwcat_ = FWCAT_UNDEF);
    public:
    
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

    private:

      /// Load a list of registered firmware from a 
      void _load_db_(const std::string &);

      /// Initialization operations
      void _at_init_();

      /// Termination operations
      void _at_reset_();
   
    private:

      // Management:
      bool _initialized_ = false; ///< Initialization flag

      // Configuration:
      std::set<firmware_category_type> _supported_firmware_categories_; ///< List of supported firmware categories 
      std::set<std::string> _firmware_db_paths_; ///!< Paths to the firmware repository

      // Working:
      firmware_db_type _db_; ///< Dictionary of available firmware
      firmware_pool_type _instances_; ///< Dictionary of instantiated firmwares
    
    };
 
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_FIRMWARE_BUILDER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
