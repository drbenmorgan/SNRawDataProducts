//! \file  snfee/model/fee_service.h
//! \brief Service for the description of the SuperNEMO front-end electronics 
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

#ifndef SNFEE_MODEL_FEE_SERVICE_H
#define SNFEE_MODEL_FEE_SERVICE_H

// Standard Library:
#include <string>
#include <memory>

// Third party:
// - Bayeux:
#include <bayeux/datatools/base_service.h>

// This project:
#include <snfee/model/setup.h>

namespace snfee {
  namespace model {

    /// \brief SuperNEMO front-end electronics description service 
    class fee_service
      : public datatools::base_service
    {
    public:
    
      /// Default constructor
      fee_service();
    
      /// Destructor
      virtual ~fee_service();
    
      /// Check initialization status
      virtual bool is_initialized() const;
    
      /// Initialize the service
      virtual int initialize(const datatools::properties & /* config_ */,
                             datatools::service_dict_type & /* service_map_ */);
    
      /// Reset the service
      virtual int reset();

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;

      /// Check if setup is set
      bool has_setup() const;
    
      /// Return a non-mutable reference to the current electronics setup
      const setup & get_setup() const;
   
    private:
    
      void _at_init_(const datatools::properties & config_);

      void _at_reset_();
    
    private:

      // Management:
      bool _initialized_ = false;

      // Internal working data:
      struct work_type;
      std::unique_ptr<work_type> _work_; //!< The PIMPL-ized working zone

      // Registration macro
      DATATOOLS_SERVICE_REGISTRATION_INTERFACE(fee_service)
 
    };
  
  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_FEE_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
