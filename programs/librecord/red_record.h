//! \file snfee/io/red_record.h
//! \brief RED record
//
// Copyright (c) 2019 by François Mauger <mauger@lpccaen.in2p3.fr>

#ifndef SNFEE_IO_RED_RECORD_H
#define SNFEE_IO_RED_RECORD_H

// Standard Library:
#include <iostream>
#include <memory>

// This project:
#include <snfee/data/raw_event_data.h>
#include "rtd_record.h"

namespace snfee {
  namespace io {

    /// \brief RED record wrapper for raw event data object
    class red_record {
    public:
      red_record();

      void make_record(const int32_t run_id_,
                       const int32_t event_id_,
                       const snfee::data::timestamp& reference_time_);

      void reset();

      bool has_record() const;

      int32_t get_event_id() const;

      snfee::data::raw_event_data& grab_red();

      const snfee::data::raw_event_data& get_red() const;

      const std::shared_ptr<snfee::data::raw_event_data>& get_red_ptr() const;

      void install_rtd(
        const std::shared_ptr<const snfee::data::raw_trigger_data>& rtd_ptr_,
        const snfee::data::timestamp& timeshift_);

      void print(std::ostream& out_) const;

      friend std::ostream& operator<<(std::ostream& out_,
                                      const red_record& rec_);

    private:
      std::shared_ptr<snfee::data::raw_event_data>
        _red_; ///< Pointer to a raw event data (RED)
    };

  } // namespace io
} // namespace snfee

#endif // SNFEE_IO_RED_RECORD_H

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
