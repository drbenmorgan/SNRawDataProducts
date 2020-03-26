//! \file  snfee/redb/builder_config.h
//! \brief Builder configuration
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

#ifndef SNFEE_REDB_BUILDER_CONFIG_H
#define SNFEE_REDB_BUILDER_CONFIG_H

// Standard Library:
#include <string>
#include <vector>

// Third Party Libraries:
#include <bayeux/datatools/i_tree_dump.h>

// This project:
#include <snfee/data/utils.h>
#include <snfee/model/utils.h>

namespace snfee {
  namespace redb {

    /// \brief Configuration for the raw trigger data (RED) builder
    class builder_config : public datatools::i_tree_dumpable {
    public:
      static const uint32_t DEFAULT_RTD_BUFFER_CAPACITY = 100;

      /// \brief Input format
      enum format_type { FORMAT_UNDEF = 0, FORMAT_BOOST_SERIAL = 1 };

      static std::string format_label(const format_type);

      /// \brief Build algo
      enum build_algo_type {
        BUILD_ALGO_NONE = 0,
        BUILD_ALGO_ONETOONE = 1,
        BUILD_ALGO_STANDARD = 1,
        BUILD_ALGO_DEFAULT = BUILD_ALGO_STANDARD
      };

      static std::string algo_label(const build_algo_type);

      /// \brief Input configuration description
      struct input_config_type {
        std::string label; ///< Identification (human friendly string)
        std::string
          listname; ///< Name of a file containing the list of input RHD files
        std::vector<std::string>
          filenames;                       ///< Explicit list of input RHD files
        format_type format = FORMAT_UNDEF; ///< Format description (unused)
      };

      /// \brief Output configuration description
      struct output_config_type {
        std::string label; ///< Identification (human friendly string)
        std::vector<std::string>
          filenames; ///< Explicit list of output RED files
        std::size_t max_records_per_file =
          0; ///< Maximum number of RED records per output file
        std::size_t max_total_records =
          0; ///< Maximum total number of RED records
        bool terminate_on_overrun =
          false; ///< Flag to silently terminate the overrunning writer (don't
                 ///< throw if set, unused)
        format_type format = FORMAT_UNDEF; ///< Format description (unused)
      };

      /// Set the run ID
      void set_run_id(const int32_t rid_);

      /// Return the run ID
      int32_t get_run_id() const;

      /// Check if the input config
      bool has_input_config() const;

      /// Set the input config
      void set_input_config(const std::string& label_,
                            const std::vector<std::string>& filepaths_,
                            const format_type format_ = FORMAT_UNDEF);

      /// Set the input config
      void set_input_config(const std::string& label_,
                            const std::string& listpath_,
                            const format_type format_ = FORMAT_UNDEF);

      /// Return the input config
      const input_config_type& get_input_config() const;

      /// Check if the output config
      bool has_output_config() const;

      /// Set the output config
      void set_output_config(const std::string& label_,
                             const std::vector<std::string>& filepaths_,
                             const std::size_t max_records_per_file_,
                             const std::size_t max_total_records_,
                             const format_type format_ = FORMAT_UNDEF);

      /// Return the output config
      const output_config_type& get_output_config() const;

      /// Smart print
      ///
      /// Usage:
      /// \code
      /// snfee::redb::builder_config redBuilderCfg
      /// ...
      /// boost::property_tree::ptree poptions;
      /// poptions.put("title", "RED builder config:");
      /// poptions.put("indent", ">>> ");
      /// redBuilderCfg.print_tree(std::clog, poptions);
      /// \endcode
      virtual void print_tree(
        std::ostream& out_ = std::clog,
        const boost::property_tree::ptree& options_ = empty_options()) const;

      /// Reset the configuration
      void reset();

      /// Check the validity and completeness of the configuration
      static void check(const builder_config& cfg_);

      /// Load the configuration from a file
      static void load(const std::string& config_filename_,
                       builder_config& cfg_);

      /// Print documentation
      static void print_skel_config(std::ostream& out_,
                                    const int run_id_ = 100);

      /// Print documentation
      static void print_documentation(std::ostream& out);

    public:
      int32_t run_id = snfee::data::INVALID_RUN_ID; ///< Run identifier
      build_algo_type build_algo = BUILD_ALGO_DEFAULT;
      input_config_type input_config;   ///< Configuration for RTD input
      output_config_type output_config; ///< Configuration for RED output
      uint32_t rtd_buffer_capacity =
        DEFAULT_RTD_BUFFER_CAPACITY; ///< Input RTD buffer capacity
    };

  } // namespace redb
} // namespace snfee

#endif // SNFEE_REDB_BUILDER_CONFIG_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
