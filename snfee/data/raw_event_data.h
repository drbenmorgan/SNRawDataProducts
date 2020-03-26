//! \file  snfee/data/raw_event_data.h
//! \brief Description of the SuperNEMO raw event data
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

#ifndef SNFEE_DATA_RAW_EVENT_DATA_H
#define SNFEE_DATA_RAW_EVENT_DATA_H

// Standard Library:
#include <iostream>
#include <memory>
#include <vector>

// This project:
#include <snfee/data/raw_trigger_data.h>
#include <snfee/data/time.h>

namespace snfee {
  namespace data {

    /// \brief SuperNEMO raw event data (RED)
    class raw_event_data : public datatools::i_tree_dumpable,
                           public datatools::i_serializable {
    public:
      /// \brief Shared pointer to a raw tracker data (RTD)
      typedef std::shared_ptr<const snfee::data::raw_trigger_data>
        const_raw_trigger_data_ptr;

      /// \brief RTD record information:
      struct rtd_record_type {
        timestamp
          time_shift; ///< Time shift with respect to the reference time
                      ///< (typically in unit of 25 ns from main clock F=40 MHz)
        const_raw_trigger_data_ptr rtd; ///< Shared pointer to a RTD record

        BOOST_SERIALIZATION_BASIC_DECLARATION()
      };

      /// Check if the record is complete
      bool is_complete() const;

      //! Reset the data
      void invalidate();

      //! Check if run ID is set
      bool has_run_id() const;

      //! Return the run ID
      int32_t get_run_id() const;

      //! Set the run ID
      void set_run_id(const int32_t);

      //! Check if event ID is set
      bool has_event_id() const;

      //! Return the event ID
      int32_t get_event_id() const;

      //! Set the event ID
      void set_event_id(const int32_t);

      //! Check if reference time is set
      bool has_reference_time() const;

      //! Return the reference time
      const timestamp& get_reference_time() const;

      //! Set the reference time
      void set_reference_time(const timestamp&);

      //! Add a new RTD record in the pack
      void add_rtd(const timestamp& time_shift_,
                   const const_raw_trigger_data_ptr& rtd_ptr_);

      //! Return the number of RTD records in the pack
      std::size_t get_number_of_rtd() const;

      //! Fetch the RTD record and its characteristics from the pack
      void fetch_rtd(int index_,
                     timestamp& time_shift_,
                     const_raw_trigger_data_ptr& rtd_ptr_) const;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::data::raw_trigger_data rawEventData
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "Raw Event Data:");
      /// poptions.put("indent", ">>> ");
      /// rawEventData.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(
        std::ostream& out_ = std::clog,
        const boost::property_tree::ptree& options_ = empty_options()) const;

    private:
      int32_t _run_id_ = INVALID_RUN_ID;     ///< Run ID
      int32_t _event_id_ = INVALID_EVENT_ID; ///< Event ID
      timestamp _reference_time_; ///< Reference timestamp in the run time frame
                                  ///< (typically in unit of 25 ns from main
                                  ///< clock F=40 MHz)
      std::vector<rtd_record_type>
        _rtd_pack_; ///< Collection of handles for raw trigger data records

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  } // namespace data
} // namespace snfee

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snfee::data::raw_event_data,
                        "snfee::data::raw_event_data")

#endif // SNFEE_DATA_CALO_HIT_RECORD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
