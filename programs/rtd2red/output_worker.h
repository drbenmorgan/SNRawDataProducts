//! \file  snfee/redb/output_worker.h
//! \brief RED output worker
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

#ifndef SNFEE_REDB_OUTPUT_WORKER_H
#define SNFEE_REDB_OUTPUT_WORKER_H

// Standard Library:
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/logger.h>

// This project:
#include <snfee/io/multifile_data_writer.h>
#include <snfee/redb/builder_config.h>
#include <snfee/redb/red_buffer.h>

namespace snfee {
  namespace redb {

    /// \brief RED output worker
    struct output_worker {

      /// Constructor
      output_worker(std::mutex& omtx_,
                    red_buffer& obuf_,
                    const builder_config::output_config_type& oconfig_,
                    const datatools::logger::priority logging_ =
                      datatools::logger::PRIO_FATAL);

      /// Destructor
      ~output_worker();

      /// Request stop
      void stop();

      bool is_stopped() const;

      std::size_t get_records_counter() const;

      std::size_t get_stored_records_counter() const;

      /// Run
      void run();

    private:
      std::mutex& _mtx_; ///< Mutex for access to the output RED buffer
      red_buffer& _buf_; ///< Handle to the output RED buffer
      std::shared_ptr<snfee::io::multifile_data_writer>
        _pwriter_;                 ///< Data writer
      bool _stop_request_ = false; ///< Thread stop request
      datatools::logger::priority _logging_ =
        datatools::logger::PRIO_FATAL;   ///< Logging priority
      std::size_t _records_counter_ = 0; ///< Counter of processed RED records
      std::size_t _stored_records_counter_ =
        0; ///< Counter of stored RED records
    };

  } // namespace redb
} // namespace snfee

#endif // SNFEE_REDB_RED_BUFFER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
