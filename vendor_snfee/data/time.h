//! \file  snfee/data/time.h
//! \brief Time and clock utilities 
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

#ifndef SNFEE_DATA_TIME_H
#define SNFEE_DATA_TIME_H
 
// Standard Library:
#include <string>
#include <iostream>
#include <limits>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux:
#include <bayeux/datatools/serialization_macros.h>

namespace snfee {
  namespace data {

    /// \brief Clock types implemented in front-end electronics
    enum clock_type {
      CLOCK_UNDEF   = 0, ///< Undefined clock
      CLOCK_40MHz   = 1, ///< Main wall clock
      CLOCK_80MHz   = 2, ///< Tracker FEAST ASIC clock
      CLOCK_160MHz  = 4, ///< Calorimeter SAMLONG ASIC clock
      CLOCK_2560MHz = 64 ///< Calorimeter waveform sampling clock
    };
    
    std::string clock_label(const clock_type);

    uint32_t clock_comparison_factor(const clock_type);

    // Special tick values:
    //
    //            -inf                  zero                   +inf
    //    ! ~ ~ ~ ~ ]--------------------0----------------------[ ~ ~ ~> ticks
    // invalid
    //
    static const int64_t PLUS_INFINITY_TICKS  = std::numeric_limits<int64_t>::max();
    static const int64_t MINUS_INFINITY_TICKS = std::numeric_limits<int64_t>::min() + 1;
    static const int64_t INVALID_TICKS        = std::numeric_limits<int64_t>::min();
    static const int64_t ZERO_TICKS           = 0;
    
    /// \brief Digitized timestamp
    class timestamp
    {
    public:
      
      timestamp() = default;
       
      timestamp(const int64_t ticks_);
       
      timestamp(const clock_type clock_, const int64_t ticks_);

      void invalidate();
      
      bool is_valid() const;

      void set_clock(const clock_type clock_);
      
      clock_type get_clock() const;

      bool is_clock_40MHz() const;

      bool is_clock_80MHz() const;

      bool is_clock_160MHz() const;

      bool is_clock_2560MHz() const;
 
      void set_ticks(const int64_t ticks_);
     
      int64_t get_ticks() const;

      bool operator<(const timestamp & other_) const;

      friend std::ostream & operator<<(std::ostream & out, const timestamp & ts_);

      /// Comparison function
      ///
      /// @param ts1_ first timestamp to be compared
      /// @param ts2_ second timestamp to be compared
      /// @param strict_
      ///
      ///         * if true, a strict comparison is used based
      ///           on valid timestamps with the same clock
      ///         * if false, lazy comparision is used based
      ///           on valid timestamps with possibly different clocks
      ///           with precedence on the slower clock
      ///
      /// @return  -2 if ts1_ and ts2_ are not comparable_
      ///          -1 if ts1_ < ts2_
      ///           0 if ts1_ == ts2_      
      ///          +1 if ts1_ > ts2_      
      ///
      static int compare(const timestamp & ts1_, const timestamp & ts2_, bool strict_ = false);
      
    private:

      clock_type _clock_ = CLOCK_UNDEF;
      int64_t    _ticks_ = INVALID_TICKS;

      BOOST_SERIALIZATION_BASIC_DECLARATION()
      
    };
    
  } // namespace data
} // namespace snfee

#endif // SNFEE_DATA_TIME_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
