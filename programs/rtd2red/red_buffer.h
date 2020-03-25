//! \file  snfee/redb/red_buffer.h
//! \brief RED output buffer
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

#ifndef SNFEE_REDB_RED_BUFFER_H
#define SNFEE_REDB_RED_BUFFER_H

// Standard Library:
#include <deque>
#include <iostream>
#include <set>

// This project:
#include <snfee/data/raw_trigger_data.h>
#include <snfee/data/utils.h>
#include "red_record.h"

namespace snfee {
  namespace redb {

    /// \brief RED output buffer
    struct red_buffer {

      /// Constructor
      red_buffer();

      /// Destructor
      ~red_buffer();

      /// Terminate the buffer
      void terminate();

      /// Check if the buffer is terminated
      bool is_terminated();

      /// Check if the buffer is empty
      bool is_empty();

      /// Insert a new RED record in the buffer
      void push_record(const snfee::io::red_record& rec_);

      /// Pop a RED record from the buffer
      snfee::io::red_record pop_record();

      /// Print
      void print(std::ostream& out_) const;

    private:
      int32_t _front_event_id_ = -1;
      std::deque<snfee::io::red_record> _red_recs_;
      bool _terminated_ = false;
    };

  } // namespace redb
} // namespace snfee

#endif // SNFEE_REDB_RED_BUFFER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
