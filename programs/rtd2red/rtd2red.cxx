// Standard library:
#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// Third party:
// - Bayeux:
#include <bayeux/datatools/io_factory.h>
#include <bayeux/datatools/logger.h>
// - Boost:
#include <boost/program_options.hpp>

// This project:
#include <snfee/redb/builder.h>
#include <snfee/utils.h>

struct app_params_type {
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  std::string config_filename;
  std::string report_filename;
  int32_t run_id = snfee::data::INVALID_RUN_ID;
  std::size_t max_total_records = 0;
  uint32_t rtd_buffer_capacity = 0;
  uint32_t skel_run_id = 100;
};

int
main(int argc_, char** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    app_params_type app_params;

    // Parse options:
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()("help,h", "produce help message")

      ("help-config,G",
       "produce documentation about configuration or the merge")

        ("skel-config,K", "produce a skeleton configuration file")

          ("skel-run-id",
           po::value<uint32_t>(&app_params.skel_run_id)->value_name("number"),
           "set the run ID (>=100) for the production of the skeleton "
           "configuration file")

            ("logging,L",
             po::value<std::string>()->value_name("level"),
             "logging priority")

              ("config,c",
               po::value<std::string>(&app_params.config_filename)
                 ->value_name("path"),
               "set the configuration filename")

                ("report,r",
                 po::value<std::string>(&app_params.report_filename)
                   ->value_name("path"),
                 "set the report filename")

                  ("run-id,R",
                   po::value<int32_t>(&app_params.run_id)->value_name("number"),
                   "set the run ID (override value from the config file)")

      // ("force-complete-red,C",
      //   po::value<bool>(&app_params.force_complete_red)
      //  ->default_value(false)
      //  ->zero_tokens(),
      //  "force complete RTD on output (override value from the config file,
      //  expert)")

      ("max-total-records,M",
       po::value<std::size_t>(&app_params.max_total_records)
         ->value_name("number"),
       "set the maximum number of RED records to be generated (expert)")

        ("rtd-buffer-capacity",
         po::value<uint32_t>(&app_params.rtd_buffer_capacity)
           ->value_name("number"),
         "set the capacity of RTD input buffers (expert)")

      ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_).options(opts).run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout
        << "snfee-rtd2red : "
        << "Merge raw trigger data file (RTD) into raw event data file (RED)"
        << std::endl
        << std::endl;
      std::cout
        << "\n"
           "  +----------------+         +-----------+       +-----------+\n"
           "  |   RTD records  |-------->|  builder  |------>|    RED    |\n"
           "  +----------------+         +-----------+       +-----------+\n"
           "\n";
      std::cout << std::endl;
      std::cout << "Usage : " << std::endl << std::endl;
      std::cout << "  snfee-rtd2red [OPTIONS]" << std::endl << std::endl;
      std::cout << opts << std::endl;
      std::cout << "Example : " << std::endl << std::endl;
      std::cout << "  snfee-rtd2red \\\n";
      std::cout << "    --run-id 8 \\\n";
      std::cout << "    --config \"rtd2red_run-8.conf\" \\\n";
      std::cout << "    --report \"rtd2red_run-8.log\"";
      std::cout << std::endl << std::endl;
      std::cout << "  snfee-rtd2red --help-config > snfee-rtd2red_config.rst\n";
      std::cout << std::endl << std::endl;
      std::cout << "  snfee-rtd2red \\\n";
      std::cout << "    --skel-config  \\\n";
      std::cout << "    --skel-run-id 100 \\\n";
      std::cout << "    > snfee-rtd2red.conf \\\n";
      std::cout << std::endl << std::endl;
      return (-1);
    }

    // Use command line arguments :
    if (vm.count("help-config")) {
      snfee::redb::builder_config::print_documentation(std::cout);
      return (-1);
    }

    // Use command line arguments :
    if (vm.count("skel-config")) {
      snfee::redb::builder_config::print_skel_config(std::cout,
                                                     app_params.skel_run_id);
      return (-1);
    }

    // Use command line arguments :
    if (vm.count("logging")) {
      std::string logging_repr = vm["logging"].as<std::string>();
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Logging repr. = '" <<
      // logging_repr << "'");
      app_params.logging = datatools::logger::get_priority(logging_repr);
      DT_THROW_IF(app_params.logging == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority '"
                    << vm["logging"].as<std::string>() << "'!");
    }

    // Checks:
    DT_THROW_IF(app_params.config_filename.empty(),
                std::logic_error,
                "Missing configuration filename!");

    snfee::redb::builder_config redBuilderCfg;
    std::string redBuilderCfgFilename = app_params.config_filename;
    snfee::redb::builder_config::load(redBuilderCfgFilename, redBuilderCfg);
    if (app_params.run_id != snfee::data::INVALID_RUN_ID) {
      if (redBuilderCfg.run_id != snfee::data::INVALID_RUN_ID) {
        DT_LOG_WARNING(app_params.logging,
                       "Override run ID " << redBuilderCfg.run_id
                                          << " from the configuration file.");
        DT_LOG_WARNING(app_params.logging,
                       "Run ID is now : " << app_params.run_id);
      }
      redBuilderCfg.run_id = app_params.run_id;
    }

    if (app_params.max_total_records != 0) {
      if (redBuilderCfg.output_config.max_total_records != 0) {
        DT_LOG_WARNING(app_params.logging,
                       "Override max number of RED records "
                         << redBuilderCfg.output_config.max_total_records
                         << " from the configuration file.");
        DT_LOG_WARNING(
          app_params.logging,
          "Max number of RED records : " << app_params.max_total_records);
      }
      redBuilderCfg.output_config.max_total_records =
        app_params.max_total_records;
    }

    if (app_params.rtd_buffer_capacity != 0) {
      if (redBuilderCfg.rtd_buffer_capacity != 0) {
        DT_LOG_WARNING(app_params.logging,
                       "Override RTD input buffer capacity "
                         << redBuilderCfg.rtd_buffer_capacity
                         << " from the configuration file.");
        DT_LOG_WARNING(app_params.logging,
                       "RTD input buffer capacity is now : "
                         << app_params.rtd_buffer_capacity);
      }
      redBuilderCfg.rtd_buffer_capacity = app_params.rtd_buffer_capacity;
    }

    // Check the configuration:
    snfee::redb::builder_config::check(redBuilderCfg);
    {
      // Print:
      boost::property_tree::ptree options;
      options.put("title", "RED Builder Configuration: ");
      redBuilderCfg.print_tree(std::clog, options);
    }

    // The RED builder:
    snfee::redb::builder redBuilder;
    redBuilder.set_logging(app_params.logging);
    redBuilder.set_config(redBuilderCfg);
    {
      // Print:
      boost::property_tree::ptree poptions;
      poptions.put("title", "RED builder:");
      redBuilder.print_tree(std::clog, poptions);
    }

    // Run:
    redBuilder.initialize();
    redBuilder.run();
    redBuilder.terminate();

    /*
    {
      // Results:
      std::ostream * redBuilderResultsOut = &std::cout;
      std::unique_ptr<std::ofstream> redBuilderResultsOutfile;
      if (!app_params.report_filename.empty()) {
        std::string redBuilderResultsFilename = app_params.report_filename;
        datatools::fetch_path_with_env(redBuilderResultsFilename);
        redBuilderResultsOutfile.reset(new
    std::ofstream(redBuilderResultsFilename.c_str()));
        DT_THROW_IF(!redBuilderResultsOutfile, std::logic_error,
                    "Cannot open result file '" << redBuilderResultsFilename <<
    "'!"); redBuilderResultsOut = redBuilderResultsOutfile.get();
      }
      const auto & redBuilderResults = redBuilder.get_results();
      int i = 0;
      *redBuilderResultsOut << "Results :" << std::endl;
      for (const auto & res : redBuilderResults) {
        *redBuilderResultsOut << "- Worker #" << i;
        if (res.category == snfee::redb::builder::WORKER_INPUT_RHD) {
          *redBuilderResultsOut << " (input RTD";
          *redBuilderResultsOut << ",crate=" <<
    snfee::model::crate_model_label(res.crate_model); *redBuilderResultsOut <<
    ")"; } else if (res.category == snfee::redb::builder::WORKER_OUTPUT_RED) {
          *redBuilderResultsOut << " (output RED)";
        }
        *redBuilderResultsOut << " : " << std::endl;
        *redBuilderResultsOut << "   - Processed records : " <<
    res.processed_records_counter1; *redBuilderResultsOut << std::endl; if
    (res.category == snfee::redb::builder::WORKER_OUTPUT_RED) {
          *redBuilderResultsOut << "   - Stored records    : " <<
    res.processed_records_counter2; *redBuilderResultsOut << std::endl;
        }
        i++;
      }
    }
    */
  }
  catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
