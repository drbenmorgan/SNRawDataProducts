//! \file  snfee/redb/input_worker.h
//! \brief Builder's input worker
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

#ifndef SNFEE_REDB_INPUT_WORKER_H
#define SNFEE_REDB_INPUT_WORKER_H

// Standard Library:
#include <fstream>
#include <iostream>
#include <memory>
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
#include <snfee/io/multifile_data_reader.h>
#include <snfee/redb/builder_config.h>
#include <snfee/redb/rtd_buffer.h>

namespace snfee {
  namespace redb {

    /// \brief Builder's input worker for RTD records
    struct input_worker {
      /// Request stop
      void stop();

      /// Check if the worker is stopped
      bool is_stopped() const;

      /// Constructor
      input_worker(const int id_,
                   std::mutex& imtx_,
                   snfee::redb::rtd_buffer& ibuf_,
                   const builder_config::input_config_type& iconfig_,
                   const datatools::logger::priority logging_ =
                     datatools::logger::PRIO_FATAL);

      /// Destructor
      ~input_worker();

      /// Run
      void run();

      /// Print
      void print(std::ostream& out_) const;

      /// Return the counter of input records
      std::size_t get_records_counter() const;

      // Configuration:
      datatools::logger::priority _logging_; ///< Logging priority
      int _id_ = -1;                         ///< Identifier of the input worker
      std::mutex&
        _mtx_; ///< Handle to the mutex associated to the input RTD buffer
      rtd_buffer& _buf_; ///< Handle to the input RTD buffer
      bool _accept_unsorted_input_ = false;
      std::shared_ptr<snfee::io::multifile_data_reader>
        _preader_; ///< Data reader

      // Working:
      bool _stop_request_ = false;       ///< Control stop
      std::size_t _records_counter_ = 0; ///< Counter of processed RTD records

    }; // end of struct input_worker

  } // namespace redb
} // namespace snfee

#endif // SNFEE_REDB_BUILDER_CONFIG_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
