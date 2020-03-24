//! \file  snfee/model/setup.h
//! \brief Description of the SuperNEMO front-end setup 
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

#ifndef SNFEE_MODEL_SETUP_H
#define SNFEE_MODEL_SETUP_H

// Standard library:
#include <string>
#include <map>
#include <memory>

// Third party:
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/logger.h>

// This project:
#include <snfee/model/crate.h>
#include <snfee/model/crate_builder.h>

namespace snfee {
  namespace model {

    /// \brief Front-end electronics setup model
    class setup
      : public datatools::i_tree_dumpable
    {
    public:

      struct crate_entry {
        std::string           name;
        crate_model_type      model;
        datatools::properties config;
      };
    
      setup();

      virtual ~setup();

      bool has_name() const;
    
      void set_name(const std::string &);

      const std::string & get_name() const;

      bool has_tag() const;

      void set_tag(const std::string &);

      const std::string & get_tag() const;
    
      void add_crate(const crate_ptr & crate_, const std::string & name_ = "");

      bool has_crate(const std::string & name_) const;

      bool has_crate(const crate_model_type model_, const int id_) const;

      const crate & get_crate(const std::string &) const;

      crate & grab_crate(const std::string &);
  
      const crate & get_crate(const crate_model_type model_,
                              const int id_) const;
 
      crate & grab_crate(const crate_model_type model_,
                         const int id_);

      std::set<std::string> get_crate_names() const;
    
      void construct_paths(std::set<path_type> & paths_,
                           const std::string & filter_ = "") const;

      const crate & get_crate(const path_type & path_) const;
    
      const base_board & get_board(const path_type & path_) const;
    
      const fpga & get_fpga(const path_type & path_) const;
    
      const register_description & get_register(const path_type & path_) const;

      bool is_crate_layout_locked() const;

      void lock_crate_layout();

      void unlock_crate_layout();

      /// Check initialization flag
      bool is_initialized() const;

      /// Standalone initialization 
      void initialize_simple();

      /// Initialization from a set of configuration parameters
      void initialize(const datatools::properties & config_,
                      const crate_builder * cb_ = nullptr);

      /// Terminate
      void reset();

      /// Print the setup structure
      ///
      /// Usage:
      /// \code
      /// snfesetup::setup mySetup
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Setup");
      /// poptions.put("indent", ">>> ");
      /// mySetup.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

    public:
    
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

    private:

      // Management:
      bool _initialized_ = false;

      // Configuration:
      std::string _name_; //!< Name of the experimental setup
      std::string _tag_;  //!< Tag of the configuration
      std::vector<crate_entry> _crate_entries_; //!< Crates configuration
    
      // Usage:
      bool _crate_layout_locked_ = false;
      std::map<std::string, crate_ptr> _crates_; 
     
    };
  
    typedef std::shared_ptr<setup> setup_ptr;
  
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_SETUP_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
