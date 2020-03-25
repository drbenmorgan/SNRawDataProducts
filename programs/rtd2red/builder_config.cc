// Ourselves:
#include "builder_config.h"

// Standard Library:
#include <sstream>

// This project:
#include <snfee/model/utils.h>

namespace snfee {
  namespace redb {

    // static
    std::string
    builder_config::format_label(const format_type f_)
    {
      if (f_ == FORMAT_BOOST_SERIAL) {
        return "boost::serialization";
      }
      return "";
    }

    // static
    std::string
    builder_config::algo_label(const build_algo_type a_)
    {
      if (a_ == BUILD_ALGO_ONETOONE) {
        return "one-to-one";
      }
      if (a_ == BUILD_ALGO_STANDARD) {
        return "standard";
      }
      return "";
    }

    builder_config::builder_config() { return; }

    builder_config::~builder_config() { return; }

    void
    builder_config::set_run_id(const int32_t rid_)
    {
      run_id = rid_;
      return;
    }

    int32_t
    builder_config::get_run_id() const
    {
      return run_id;
    }

    bool
    builder_config::has_output_config() const
    {
      return !output_config.filenames.size();
    }

    void
    builder_config::set_output_config(
      const std::string& label_,
      const std::vector<std::string>& filepaths_,
      const std::size_t max_records_per_file_,
      const std::size_t max_total_records_,
      const format_type format_)
    {
      output_config_type oc;
      oc.label = label_;
      oc.max_records_per_file = max_records_per_file_;
      oc.max_total_records = max_total_records_;
      oc.filenames = filepaths_;
      oc.format = format_;
      output_config = oc;
      return;
    }

    const builder_config::output_config_type&
    builder_config::get_output_config() const
    {
      return output_config;
    }

    bool
    builder_config::has_input_config() const
    {
      return !input_config.filenames.size();
    }

    const builder_config::input_config_type&
    builder_config::get_input_config() const
    {
      return input_config;
    }

    void
    builder_config::set_input_config(const std::string& label_,
                                     const std::vector<std::string>& filepaths_,
                                     const format_type format_)
    {
      input_config_type ic;
      ic.label = label_;
      ic.filenames = filepaths_;
      ic.format = format_;
      input_config = ic;
      return;
    }

    void
    builder_config::set_input_config(const std::string& label_,
                                     const std::string& listpath_,
                                     const format_type format_)
    {
      input_config_type ic;
      ic.label = label_;
      ic.listname = listpath_;
      ic.format = format_;
      input_config = ic;
      return;
    }

    // virtual
    void
    builder_config::print_tree(
      std::ostream& out_,
      const boost::property_tree::ptree& options_) const
    {
      base_print_options popts;
      popts.configure_from(options_);

      std::ostringstream outs;
      if (popts.title.length()) {
        outs << popts.indent << popts.title << std::endl;
      }

      outs << popts.indent << tag << "Run ID : " << run_id << std::endl;

      outs << popts.indent << tag << "Build algo : " << algo_label(build_algo)
           << std::endl;

      outs << popts.indent << tag << "Input config : " << std::endl;

      outs << popts.indent << skip_tag << tag << "Label : '"
           << input_config.label << "'" << std::endl;

      outs << popts.indent << skip_tag << tag
           << "Filenames : " << input_config.filenames.size() << std::endl;

      for (int ifilename = 0; ifilename < (int)input_config.filenames.size();
           ifilename++) {
        outs << popts.indent << skip_tag << skip_tag;
        if ((ifilename + 1) == (int)input_config.filenames.size()) {
          outs << last_tag;
        } else {
          outs << tag;
        }
        outs << "Filename #" << ifilename << " : '"
             << input_config.filenames[ifilename] << "'";
        outs << std::endl;
      }

      outs << popts.indent << skip_tag << last_tag << "Format : '"
           << format_label(input_config.format) << "'" << std::endl;

      outs << popts.indent << tag << "Output config : " << std::endl;

      outs << popts.indent << skip_tag << tag << "Label : '"
           << output_config.label << "'" << std::endl;

      outs << popts.indent << skip_tag << tag
           << "Filenames : " << output_config.filenames.size() << std::endl;

      for (int ifilename = 0; ifilename < (int)output_config.filenames.size();
           ifilename++) {
        outs << popts.indent << skip_tag << skip_tag;
        if ((ifilename + 1) == (int)output_config.filenames.size()) {
          outs << last_tag;
        } else {
          outs << tag;
        }
        outs << "Filename #" << ifilename << " : '"
             << output_config.filenames[ifilename] << "'";
        outs << std::endl;
      }

      outs << popts.indent << skip_tag << tag
           << "Max records/file : " << output_config.max_records_per_file
           << std::endl;

      outs << popts.indent << skip_tag << tag
           << "Max total records : " << output_config.max_total_records
           << std::endl;

      outs << popts.indent << skip_tag << tag
           << "Terminate/overrun : " << std::boolalpha
           << output_config.terminate_on_overrun << std::endl;

      outs << popts.indent << skip_tag << last_tag << "Format : '"
           << format_label(output_config.format) << "'" << std::endl;

      outs << popts.indent << skip_tag << last_tag
           << "RTD buffer capacity : " << rtd_buffer_capacity << std::endl;

      out_ << outs.str();
      return;
    }

    void
    builder_config::reset()
    {
      run_id = snfee::data::INVALID_RUN_ID;
      build_algo = BUILD_ALGO_DEFAULT;
      {
        input_config_type empty;
        input_config = empty;
      }
      {
        output_config_type empty;
        output_config = empty;
      }
      return;
    }

    void
    builder_config::check(const builder_config& cfg_)
    {
      DT_THROW_IF(cfg_.run_id == snfee::data::INVALID_RUN_ID,
                  std::logic_error,
                  "Missing run ID!");
      DT_THROW_IF(cfg_.input_config.filenames.size() == 0 and
                    cfg_.input_config.listname.empty(),
                  std::logic_error,
                  "Missing RTD input files!");
      DT_THROW_IF(cfg_.input_config.filenames.size() == 0,
                  std::logic_error,
                  "Missing RED output files!");
      return;
    }

    void
    builder_config::load(const std::string& redb_config_filename_,
                         builder_config& cfg_)
    {
      // Clear the config:
      cfg_.reset();

      std::string filename = redb_config_filename_;
      datatools::fetch_path_with_env(filename);
      datatools::properties redb_config;
      datatools::properties::read_config(filename, redb_config);

      // Run ID:
      if (redb_config.has_key("run_id")) {
        int32_t run_id = snfee::data::INVALID_RUN_ID;
        run_id = redb_config.fetch_integer("run_id");
        cfg_.set_run_id(run_id);
      }

      // Algo:
      if (redb_config.has_key("build_algo")) {
        std::string algo_str = redb_config.fetch_string("build_algo");
        if (algo_str == algo_label(BUILD_ALGO_STANDARD)) {
          cfg_.build_algo = BUILD_ALGO_STANDARD;
        } else {
          DT_THROW(std::logic_error,
                   "Invalid build algo label '" << algo_str << "'!");
        }
      }

      {
        // RTD input:
        input_config_type icfg;
        {
          std::string key = "rtd.input.label";
          if (redb_config.has_key(key)) {
            icfg.label = redb_config.fetch_string(key);
          }
        }
        {
          std::string key = "rtd.input.listname";
          if (redb_config.has_key(key)) {
            icfg.listname = redb_config.fetch_path(key);
          }
        }
        {
          std::string key = "rtd.input.filenames";
          if (redb_config.has_key(key)) {
            redb_config.fetch(key, icfg.filenames);
          }
        }
        cfg_.input_config = icfg;
      }

      {
        // RED output:
        output_config_type ocfg;
        {
          std::string key = "rtd.output.label";
          if (redb_config.has_key(key)) {
            ocfg.label = redb_config.fetch_string(key);
          }
        }
        {
          std::string key = "rtd.output.filenames";
          if (redb_config.has_key(key)) {
            redb_config.fetch(key, ocfg.filenames);
          }
        }
        {
          std::string key = "rtd.output.max_records_per_file";
          if (redb_config.has_key(key)) {
            ocfg.max_records_per_file = redb_config.fetch_positive_integer(key);
          }
        }
        {
          std::string key = "rtd.output.max_total_records";
          if (redb_config.has_key(key)) {
            ocfg.max_total_records = redb_config.fetch_positive_integer(key);
          }
        }
        {
          std::string key = "rtd.output.terminate_on_overrun";
          if (redb_config.has_key(key)) {
            ocfg.terminate_on_overrun = redb_config.fetch_boolean(key);
          }
        }
        cfg_.output_config = ocfg;
      }

      // Buffer capacity:
      if (redb_config.has_key("rtd_buffer_capacity")) {
        cfg_.rtd_buffer_capacity =
          redb_config.fetch_positive_integer("rtd_buffer_capacity");
      }

      return;
    }

    /// Print skeleton configuration
    // static
    void
    builder_config::print_skel_config(std::ostream& out_, const int run_id_)
    {
      int run_id = run_id_;
      if (run_id < 100) {
        run_id = 99; // Test value
      }
      out_ << "# -*- mode: conf-unix; -*-                                 \n\n";

      out_ << "###########################################################\n"
              "#@config Configuration file for the snfee-rtd2red builder  \n"
              "###########################################################\n\n";

      out_ << "#@description Run unique ID (mandatory)                    \n"
              "run_id : integer = "
           << run_id
           << "                        \n"
              "                                                           \n";

      out_ << "###########################################################\n";
      out_
        << "#@description Explicit list of RTD input files (mandatory)     \n"
           "# This list must contains enough file to load all RTD records  \n"
           "rtd.input.filenames : string[3] as path = \\                   \n"
           "  \"snemo_run-"
        << run_id
        << "_rtd_part-0.data.gz\" \\          \n"
           "  \"snemo_run-"
        << run_id
        << "_rtd_part-1.data.gz\" \\          \n"
           "  \"snemo_run-"
        << run_id
        << "_rtd_part-2.data.gz\"             \n"
           "                                                                \n";

      out_ << "###########################################################\n";
      out_
        << "#@description Explicit list of RED output files (mandatory)     \n"
           "# This list must contains enough file to store all RED records  \n"
           "red.output.filenames : string[3] as path = \\                   \n"
           "  \"snemo_run-"
        << run_id
        << "_red_part-0.data.gz\" \\          \n"
           "  \"snemo_run-"
        << run_id
        << "_red_part-1.data.gz\" \\          \n"
           "  \"snemo_run-"
        << run_id
        << "_red_part-2.data.gz\"             \n"
           "                                                                \n"
           "#@description Maximum number of RED records per output files "
           "(optional)\n"
           "red.output.max_records_per_file : integer = 1000000                "
           "    \n"
           "                                                                   "
           "    \n"
           "#@description Maximum total number of RED records (optional)       "
           "    \n"
           "red.output.max_total_records : integer = 3000000                   "
           "    \n"
           "                                                                   "
           "    \n";
      out_ << "###########################################################\n";
      out_ << "#@description Capacity of the buffer for RTD records (optional) "
              "  \n"
              "rtd_buffer_capacity    : integer = "
           << DEFAULT_RTD_BUFFER_CAPACITY
           << "\n"
              "                                                                "
              "       \n";
      out_ << "# end.";
      ;
      return;
    }

    /// Print documentation
    // static
    void
    builder_config::print_documentation(std::ostream& out_)
    {
      out_ << "================================\n"
              "The snfee-rhd2rtd merger        \n"
              "================================\n"
              "                                \n"
              ".. contents::                   \n"
              "                                \n"
              "Configuration file for the merger \n"
              "--------------------------------- \n"
              "                                                              \n"
              "Example of a snfee-rhd2rtd configuration file for the merging \n"
              "of RHD data files produced by two different crates:           \n"
              "                                                              \n"
              " - a *calorimeter* crate with ID = 0                          \n"
              " - a *tracker* crate with ID = 0                              \n"
              "                                                              \n"
              "                                                              \n"
              "Configuration file:                                           \n"
              "                                                              \n"
              ".. code:: bash                                                \n"
              "                                                              \n"
              "   #@description Run unique ID (mandatory)                    \n"
              "   run_id : integer = 104                                     \n"
              "                                                              \n"
              "   #@description List of RHD inputs (mandatory)               \n"
              "   rhd.inputs : string[2] = \"Calo0\" \"Tracker0\"            \n"
              "                                                              \n"
              "   #@description Crate model associated to the first RHD input "
              "(mandatory)\n"
              "   rhd.inputs.Calo0.crate_model : string = \"Calorimeter\"    \n"
              "                                                              \n"
              "   #@description Crate ID associated to the first RHD input "
              "(mandatory)\n"
              "   rhd.inputs.Calo0.crate_id : integer = 0                    \n"
              "                                                              \n"
              "   #@description File with the list of RHD input files for the "
              "first RHD input   \n"
              "   # This parameter is mandatory if the below parameter "
              "'filenames' is not provided.\n"
              "   rhd.inputs.Calo0.listname : string as path = "
              "\"snemo_run-104_rhd_files.lis\" \n"
              "                                                                "
              "                \n"
              "   #@description Explicit list of RHD input files for the first "
              "RHD input \n"
              "   # This parameter is mandatory if the above parameter "
              "'listname' is not provided.\n"
              "   rhd.inputs.Calo0.filenames : string[3] as path = \\  \n"
              "     \"snemo_run-104_rhd_calo-0_part-0.data.gz\" \\    \n"
              "     \"snemo_run-104_rhd_calo-0_part-1.data.gz\" \\    \n"
              "     \"snemo_run-104_rhd_calo-0_part-2.data.gz\"       \n"
              "                                                       \n"
              "   #@description Crate model associated to the second RHD input "
              "(mandatory)      \n"
              "   rhd.inputs.Tracker0.crate_model : string = \"Tracker\"       "
              "    \n"
              "                                                                "
              "    \n"
              "   #@description Crate ID associated to the second RHD input "
              "(mandatory)         \n"
              "   rhd.inputs.Tracker0.crate_id : integer = 0                   "
              "    \n"
              "                                                                "
              "    \n"
              "   #@description File with the list of RHD input files for the "
              "second RHD input \n"
              "   # This parameter is mandatory if the below parameter "
              "'filenames' is not provided.\n"
              "   rhd.inputs.Tracker0.listname : string as path = "
              "\"snemo_run-104_rhd_files.lis\" \n"
              "                                                                "
              "                   \n"
              "   #@description Explicit list of RHD input files for the "
              "second RHD input \n"
              "   # This parameter is mandatory if the above parameter "
              "'listname' is not provided.\n"
              "   rhd.inputs.Tracker0.filenames : string[7] as path = \\       "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-0.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-1.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-2.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-3.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-4.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-5.data.gz\" \\          "
              "   \n"
              "     \"snemo_run-104_rhd_tracker-0_part-6.data.gz\"             "
              "   \n"
              "                                                                "
              "   \n"
              "   #@description Explicit list of RTD output files (mandatory)  "
              "   \n"
              "   # This list must contains enough file to store all RTD "
              "records  \n"
              "   rtd.output.filenames : string[3] as path = \\                "
              "   \n"
              "     \"snemo_run-104_rtd_part-0.data.gz\" \\                    "
              "   \n"
              "     \"snemo_run-104_rtd_part-1.data.gz\" \\                    "
              "   \n"
              "     \"snemo_run-104_rtd_part-2.data.gz\"                       "
              "   \n"
              "                                                                "
              "   \n"
              "   #@description Maximum number of RTD records per output files "
              "(optional)\n"
              "   rtd.output.max_records_per_file : integer = 1000000          "
              "          \n"
              "                                                                "
              "          \n"
              "   #@description Maximum total number of RTD records (optional) "
              "          \n"
              "   rtd.output.max_total_records : integer = 3000000             "
              "          \n"
              "                                                                "
              "          \n"
              "   #@description Capacity of the buffer for calo RHD records "
              "(optional)   \n"
              "   calo_rhd_buffer_capacity    : integer = 100                  "
              "          \n"
              "                                                                "
              "          \n"
              "   #@description Capacity of the buffer for tracker RHD records "
              "(optional)\n"
              "   tracker_rhd_buffer_capacity : integer = 500                  "
              "          \n"
              "..                                                              "
              "          \n"
              "                                                                "
              "          \n";
      return;
    }

  } // namespace redb
} // namespace snfee
