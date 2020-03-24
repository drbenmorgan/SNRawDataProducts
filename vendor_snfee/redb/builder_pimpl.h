//! \file  snfee/redb/builder_pimpl.h
//! \brief RED builder pimpl 
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

#ifndef SNFEE_REDB_BUILDER_PIMPL_H
#define SNFEE_REDB_BUILDER_PIMPL_H
 
// Standard Library:
#include <memory>
#include <mutex>

// This project:
#include <snfee/redb/rtd_buffer.h>
#include <snfee/redb/red_buffer.h>
#include <snfee/redb/input_worker.h>
#include <snfee/redb/output_worker.h>

namespace snfee {
  namespace redb {
    
    struct rtd2red_builder;
    
    /// Smart pointer to a RTD input worker
    typedef std::shared_ptr<input_worker> input_worker_ptr;

    /// Smart pointer to a RED output worker
    typedef std::shared_ptr<output_worker> output_worker_ptr;

    /// Smart pointer to a RED builder
    typedef std::shared_ptr<rtd2red_builder> rtd2red_builder_ptr;

    /// \brief Pimpl-ized private resources
    ///
    ///                               imtx               +----------------+      omtx
    ///           +------------+     +------------+      |                |     +---------+         
    /// [RHD]-->--| iworker    |-->--| ibuffer    |---->-|     builder    |-->--| obuffer |-->--[RTD]
    ///           +------------+     +------------+      |                |     +---------+
    ///                                                  +----------------+
    ///
    struct builder_pimpl_type
    {
      input_worker_ptr            iworker; ///< RHD input workers
      rtd_buffer                  ibuffer; ///< Buffer of input raw trigger data records (RTD)
      std::shared_ptr<std::mutex> imtx;    ///< Mutex for individual access to input buffer
      rtd2red_builder_ptr         builder; ///< builder of RED records from RTD records
      red_buffer                  obuffer; ///< Buffer of output raw trigger data (RTD)
      std::shared_ptr<std::mutex> omtx;    ///< Mutex for access to output buffer
      output_worker_ptr           oworker; ///< RED output worker

      builder_pimpl_type()
      {
        return;
      }
      
      friend struct builder;
    };

  } // namespace redb
} // namespace snfee

#endif // SNFEE_REDB_BUILDER_PIMPL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
