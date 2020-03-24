//! \file  snfee/model/crate_builder.h
//! \brief Special builder for the instantiation of registered SuperNEMO crate models 
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

#ifndef SNFEE_MODEL_CRATE_BUILDER_H
#define SNFEE_MODEL_CRATE_BUILDER_H

// Standard library:
#include <string>
#include <set>

// Third party:
// - Bayeux:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/logger.h>

// This project:
#include <snfee/utils.h>
//#include <snfee/model/utils.h>
#include <snfee/model/crate.h>
#include <snfee/model/board_builder.h>

namespace snfee {
  namespace model {

    /// \brief SuperNEMO front-end electronics crate builder 
    class crate_builder
      : public datatools::i_tree_dumpable
    {
    public:
   
      /// Default constructor
      crate_builder(const board_builder &);
    
      /// Destructor
      virtual ~crate_builder();

      const board_builder & get_board_maker() const;
    
      /// Check is a crate model is supported
      bool is_crate_model_supported(const crate_model_type type_) const;

      /// Add a supported crate type
      void add_supported_crate_model(const crate_model_type type_);

      /// Return the set of supported crate types 
      const std::set<crate_model_type> & get_supported_crate_models() const;

      /// Crate factory method
      virtual crate_ptr make_crate(const crate_model_type type_,
                                   const datatools::properties & config_ = empty_config()) const;


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

    public:
    
      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

    private:

      // Management:
      const board_builder & _board_maker_;
      bool _initialized_ = false; ///< Initialization flag

      // Configuration:
      std::set<crate_model_type> _supported_crate_models_; ///< List of supported crate models 
 
    };
 
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_CRATE_BUILDER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
