//! \file  snfee/redb/rtd2red_builder.h
//! \brief Raw event data builder from RTD
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

#ifndef SNFEE_REDB_RTD2RED_BUILDER_H
#define SNFEE_REDB_RTD2RED_BUILDER_H
 
// Standard Library:
#include <set>
#include <deque>
#include <iostream>

// This project:
#include <snfee/data/raw_trigger_data.h>
#include <snfee/data/utils.h>
#include <snfee/redb/builder_pimpl.h>
#include <snfee/redb/builder_config.h>

namespace snfee {
  namespace redb {
    
    /// \brief RTD to RED builder
    struct rtd2red_builder
    {

      /// Constructor
      rtd2red_builder(builder_pimpl_type & pimpl_,
                      const int32_t run_id_,
                      const builder_config::build_algo_type algo_,
                      const datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);
            
      /// Request stop
      void stop();

      /// Check if the builder is stopped
      bool is_stopped() const;

      /// Return the counter of RED records
      std::size_t get_red_records_counter() const;

      /// Return the minimum trigger ID from the input buffer
      int32_t get_minimum_trigger_id_from_input_buffer() const;

      /// Return the next trigger ID from the input buffer
      int32_t get_next_trigger_id_from_input_buffer() const;

      /// Run
      void run();

    private:

      /// Run in standard build algo
      void _run_standard_algo_();

      /// Run in one2one build algo
      void _run_one2one_algo_();
 
    private:

      // Configuration:
      builder_pimpl_type &        _pimpl_; ///< Builder internal working stuff
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      int32_t                     _run_id_ = snfee::data::INVALID_RUN_ID;
      builder_config::build_algo_type _algo_ = builder_config::BUILD_ALGO_NONE;
      
      // Working:
      bool                 _stop_request_ = false;    ///< Thread stop request
      int32_t              _event_id_ = snfee::data::INVALID_EVENT_ID;
      std::size_t          _red_records_counter_ = 0; ///< Counter of built RED records
      
    };

  } // namespace redb
} // namespace snfee

#endif // SNFEE_REDB_RTD2RED_BUILDER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
