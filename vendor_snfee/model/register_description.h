//! \file  snfee/model/register_description.h
//! \brief Description of register published by the firmwate in a FPGA
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

#ifndef SNFEE_MODEL_REGISTER_DESCRIPTION_H
#define SNFEE_MODEL_REGISTER_DESCRIPTION_H

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/optional.hpp>
#include <boost/dynamic_bitset.hpp>
// - Bayeux:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/model/utils.h>

namespace snfee {
  namespace model {

    /// \brief Description of a register defined by some firmware code
    class register_description
      : public datatools::i_tree_dumpable
    {
    public:

      /// Maximum supported size for a register
      static const std::size_t MAX_SIZE = 64;
    
      register_description();
    
      register_description(const std::string & name_,
                           const std::string & desc_,
                           const std::string & address_,
                           const std::size_t nbits_,
                           const access_type access_,
                           const usage_type usage_,
                           const action_type action_,
                           const std::string & defval_ = "");

      virtual ~register_description();

      static bool check_overflow(const unsigned long value_,
                                 const std::size_t nbits_);
    
      void reset();
    
      bool is_valid() const;

      bool has_name() const;
    
      void set_name(const std::string & name_);

      const std::string & get_name() const;
 
      bool has_access() const;
  
      void set_access(const access_type access_);

      access_type get_access() const;

      bool is_read_only() const;

      bool is_write_only() const;

      bool is_read_write() const;
  
      bool is_readable() const;
  
      bool is_writable() const;

      bool has_usage() const;
  
      void set_usage(const usage_type usage_);

      usage_type get_usage() const;
 
      bool has_action() const;
  
      void set_action(const action_type action_);

      action_type get_action() const;
    
      bool has_description() const;
    
      void set_description(const std::string &);

      const std::string & get_description() const;

      bool has_address() const;
    
      void set_address(const std::string &);

      const std::string & get_address() const;

      bool has_nbits() const;
   
      void set_nbits(const std::size_t);
    
      std::size_t get_nbits() const;

      bool has_default_value() const;

      const std::string & get_default_value() const;

      void set_default_value(const std::string &);

      unsigned long get_default_value_as_ulong() const;
    
      boost::dynamic_bitset<> get_default_value_as_bitset() const;

      bool validate(const unsigned long value_) const;
   
      /// Print the full register's description
      virtual void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = empty_options()) const;
    
    private:

      std::string _name_;          ///< Name of the register
      std::string _description_;   ///< Description of the register
      std::string _address_;       ///< Register's (base) address
      std::size_t _nbits_    = 0;  ///< Number of bits
      access_type _access_   = ACCESS_UNDEF; ///< R/W access
      usage_type  _usage_    = USAGE_UNDEF;  ///< Usage
      action_type _action_   = ACTION_UNDEF; ///< Action
      std::string _default_value_;           ///< Default value
   
    };

  } // namespace model
} // namespace snfee

#endif // SNFEE_MODEL_REGISTER_DESCRIPTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
