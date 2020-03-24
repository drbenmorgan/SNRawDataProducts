//! \file  snfee/io/raw_record_flow_statistics.h
//! \brief Statistics on raw record flow 
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

#ifndef SNFEE_IO_RAW_RECORD_FLOW_STATISTICS_H
#define SNFEE_IO_RAW_RECORD_FLOW_STATISTICS_H

// Standard library:
#include <map>
#include <utility>
#include <iostream>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>

// This project:
#include <snfee/tools/histogram_int.h>

namespace snfee {
  namespace io {

    /// \brief Statistics on raw record flow 
    class raw_record_flow_statistics
    {
    public:

      static const size_t DEFAULT_BUFFER_SIZE = 10000;

      /// \brief Inversion record
      struct inversion_record
      {
        /// Constructor
        inversion_record(int pos1_ = -1, int pos2_ = -1, int tid1_ = -1, int tid2_ = -1);

        /// Compute the distance
        int get_dist() const;

        int pos1 = -1; ///< Reference position
        int pos2 = -1; ///< Inversion position
        int tid1 = -1; ///< Trigger ID at the reference position
        int tid2 = -1; ///< Trigger ID at the inversion position
        
      };

      /// Constructor
      raw_record_flow_statistics(const size_t buffer_size_ = DEFAULT_BUFFER_SIZE, const bool debug_ = false);
 
      /// Return logging priority
      datatools::logger::priority get_logging() const;

      /// Set logging priority
      void set_logging(const datatools::logger::priority);

      /// Return the minimum trigger ID
      int get_min_trigger_id();

      /// Return the maximum detected inversion distance
      int get_max_inversion_distance();

      /// Add a new trigger ID from a position in the flow
      void add(const int pos_, const int tid_);

      /// Return the histogram of distances of inversion
      const snfee::tools::histogram_int & get_histogram_inversions() const;
      
    private:

      /// Private initialization
      void _init_();

    private:
      
      // Configuration:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
      int _buffer_size_ = DEFAULT_BUFFER_SIZE; ///< Size of the buffer

      // Work:
      std::map<int,int>              _first_positions_; ///< Buffer of record of trigger IDs' first positions
      std::map<int,inversion_record> _inversions_; ///< List of inversions
      int                            _max_inversion_distance_ = -1; ///< Maximum detected inversion distance
      snfee::tools::histogram_int    _hdistances_; ///< Histogram of distances of inversion
      
    };
    
  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RAW_RECORD_FLOW_STATISTICS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
