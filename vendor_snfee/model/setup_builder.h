//! \file  snfee/model/setup_builder.h
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
#include <memory>

// Third party:
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>
#include <bayeux/datatools/logger.h>

// This project:
#include <snfee/model/setup.h>

namespace snfee {
  namespace model {

    /// \brief Front-end electronics setup model
    class setup_builder
      : public datatools::i_tree_dumpable
    {
    public:

      static const INVALID_SETUP_ID = -1;
      static const DEFAULT_SETUP_ID =  0;

      /// Constructor
      setup_builder(const int setup_id_);

      /// Destructor
      ~setup_builder();

      int get_setup_id() const;

      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;

      std::shared_ptr<setup> build() const;

    protected:

      void _build_prod_supernemo(setup & setup_) const;
      
    private:

      int _setup_id_ = -1;

    };
 
 
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_SETUP_BUILDER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
