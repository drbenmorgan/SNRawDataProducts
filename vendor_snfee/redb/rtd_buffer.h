//! \file  snfee/redb/rtd_buffer.h
//! \brief RTD input buffer 
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

#ifndef SNFEE_REDB_RTD_BUFFER_H
#define SNFEE_REDB_RTD_BUFFER_H
 
// Standard Library:
#include <set>
#include <deque>
#include <iostream>

// This project:
#include <snfee/data/raw_trigger_data.h>
#include <snfee/data/utils.h>
#include <snfee/io/rtd_record.h>

namespace snfee {
  namespace redb {

    /// \brief RTD input buffer
    ///
    /// \code
    /// min_popping_trig_ids = 2
    /// current_trig_id      = 4
    /// rtd_recs             = [TRIGID=4][TRIGID=5][TRIGID=6][TRIGID=7][TRIGID=8][TRIGID=9]
    /// trig_ids             = {4,5,7,8}
    /// terminated           = false
    /// \endcode
    ///
    struct rtd_buffer
    {

      /// Constructor
      rtd_buffer(const uint32_t capacity_ = 100,
                 const std::size_t min_popping_trig_ids_ = 1);
            
      /// Destructor
      ~rtd_buffer();

      /// Reset the number of stored RTD items
      std::size_t size() const;
      
      /// Reset the capacity of the buffer
      std::size_t get_capacity() const;
             
      /// Reset the RTD buffer
      void reset();
      
      /// Check if the buffer is finished
      bool is_finished() const;
      
      /// Check if the buffer is finished
      int32_t get_front_trig_id() const;
      
      /// Check if a new RTD record can be pushed in the buffer
      bool can_push() const;

      /// Insert a new RTD record in the buffer
      void insert_record(const snfee::io::rtd_record rtd_rec_);
            
      /// Pop a RTD record from the buffer
      snfee::io::rtd_record pop_record();

      /// Terminate the buffer
      void terminate();
      
      /// Check if the buffer is terminated
      bool is_terminated() const;
    
      /// Check if the buffer is empty
      bool is_empty() const;

      /// Return the trigger ID of the next poppable RTD record
      int32_t get_next_poppable_trig_id() const;

      /// Check if a RTD record can be popped from the buffer
      bool can_be_popped() const;

      /// Print
      void print(std::ostream & out_) const;
        
    private:

      // Configuration:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      uint32_t               _capacity_ = 0; ///< FIFO max capacity
      std::size_t            _min_popping_trig_ids_ = 1; ///< Minimal number of trigger IDs needed before popping
      std::deque<snfee::io::rtd_record> _rtd_recs_; ///< FIFO of RTD records 
      std::set<int32_t>      _trig_ids_; ///< Set of trigger IDs associated to currently stored RTD records
      int32_t                _front_trig_id_ = snfee::data::INVALID_TRIGGER_ID; ///< Front record trigger ID
      bool                   _terminated_ = false; ///< Input source is terminated and cannot push more RTD records

    };
 
  } // namespace redb
} // namespace snfee

#endif // SNFEE_REDB_RTD_BUFFER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
