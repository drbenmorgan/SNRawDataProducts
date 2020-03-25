//! \file  snfee/data/utils.h
//! \brief Description of the SuperNEMO trigger raw record
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

#ifndef SNFEE_DATA_UTILS_H
#define SNFEE_DATA_UTILS_H

namespace snfee {
  namespace data {

    static const int32_t INVALID_RUN_ID = -1;
    static const int32_t INVALID_TRIGGER_ID = -1;
    static const int32_t MAX_TRIGGER_ID = 16777215;
    static const int32_t INVALID_EVENT_ID = -1;
    static const int32_t INVALID_TIME_TICK = -1;

    /// \brief Comparator of trigger IDs
    ///
    /// The comparison takes into account the range of valid trigger IDs (24-bit
    /// integer) and possible overflow values above max trigger ID (0xC00000).
    struct trigger_id_comparator_type {
      static const int32_t LOWER_SAFE_TID = 4194304;
      static const int32_t UPPER_SAFE_TID = 12582912;

      bool
      operator()(const int32_t lhs_, const int32_t rhs_) const
      {

        if ((lhs_ > UPPER_SAFE_TID) && (rhs_ < LOWER_SAFE_TID)) {
          // 0----------------max 0----------------max
          // [   ][   ][   ][LHS] [RHS][   ][   ][   ]
          // <low            >up  <low            >up
          return true;
        }
        if ((lhs_ < LOWER_SAFE_TID) && (rhs_ > UPPER_SAFE_TID)) {
          // 0----------------max 0----------------max
          // [   ][   ][   ][RHS] [LHS][   ][   ][   ]
          // <low            >up  <low            >up
          return false;
        }
        return (lhs_ < rhs_);
      }
    };

  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
