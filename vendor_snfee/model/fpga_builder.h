//! \file  snfee/model/fpga_builder.h
//! \brief Special builder for the instantiation of registered SuperNEMO FPGA models 
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

#ifndef SNFEE_MODEL_FPGA_BUILDER_H
#define SNFEE_MODEL_FPGA_BUILDER_H

// Standard library:
#include <string>
#include <set>

// Third party:
// - Bayeux:
#include <bayeux/datatools/properties.h>
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/model/utils.h>
#include <snfee/model/fpga.h>

namespace snfee {
  namespace model {

    class firmware_builder;

    /// \brief SuperNEMO front-end electronics FPGA builder 
    class fpga_builder
      : public datatools::i_tree_dumpable
    {
    public:
   
      /// Default constructor
      fpga_builder(const firmware_builder & fb_);
    
      /// Destructor
      virtual ~fpga_builder();

      /// Check is a FPGA model is supported
      bool is_fpga_model_supported(const fpga_model_type & model_) const;

      /// Add a supported FPGA model
      void add_supported_fpga_model(const fpga_model_type & model_);

      /// Return the set of supported FPGA models 
      const std::set<fpga_model_type> & get_supported_fpga_models() const;

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

      /// Factory method
      virtual fpga make_fpga_index(const fpga_model_type model_,
                                   const int id_,
                                   const std::string & fwid_) const;

      /// Factory method
      virtual fpga make_fpga_unique(const fpga_model_type model_,
                                    const std::string & fwid_) const;

    private:

      /// Factory method
      virtual void _make_fpga_(fpga & pga_,
                               const fpga_model_type model_,
                               const int id_,
                               const std::string & fwid_) const;
    
    private:

      // Management:
      const firmware_builder & _fwb_;
      bool _initialized_ = false; ///< Initialization flag
    
      // Configuration:
      std::set<fpga_model_type> _supported_fpga_models_; ///< List of supported fpga models 
 
    };
 
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_FPGA_BUILDER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
