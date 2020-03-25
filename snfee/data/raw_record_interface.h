//! \file  snfee/data/raw_record_interface.h
//! \brief Interface with garanteed hit properties
//
// Copyright (c) 2019 by François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef SNFEE_DATA_RAW_RECORD_INTERFACE_H
#define SNFEE_DATA_RAW_RECORD_INTERFACE_H

// Standard library:
#include <memory>

// This project:
#include <snfee/data/has_trigger_id_interface.h>

namespace snfee {
  namespace data {

    //! \brief Abstract interface for raw hits of any type with have a unique
    //! identifier/number and a trigger ID
    class raw_record_interface : public has_trigger_id_interface {
    public:
      //! Return the trigger ID
      virtual int32_t get_hit_num() const = 0;

      //! Invalidate the data structure
      virtual void invalidate() = 0;

      //! Check if the hit number is set
      bool
      has_hit_num() const
      {
        return get_hit_num() >= 0;
      }
    };

    typedef std::shared_ptr<raw_record_interface> raw_record_ptr;

    // struct raw_record_comparator_type
    // {
    //   static const uint32_t LOWER_SAFE_TID = 0x400000;
    //   static const uint32_t UPPER_SAFE_TID = 0xC00000;
    //   raw_record_comparator_type(uint32_t);
    //   bool operator() (const snfee::data::raw_record_ptr & lhs_,
    //                    const snfee::data::raw_record_ptr & rhs_) {
    //     if ((lhs_ > UPPER_SAFE_TID) && (rhs_ < LOWER_SAFE_TID)) {
    //       return true;
    //     }
    //     return (*lhs_ < *rhs_);
    //   }
    //   uint32_t _;
    // };

  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_RAW_RECORD_INTERFACE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
