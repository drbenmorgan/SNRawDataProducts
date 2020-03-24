// Standard library:
#include <cstdio>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>

// Third party:
// - Bayeux:
#include <bayeux/datatools/logger.h>
#include <bayeux/datatools/io_factory.h>
// - Boost:
#include <boost/program_options.hpp>

// This project:
#include <snfee/utils.h>
#include <snfee/io/rhd_sorter.h>

struct app_params_type
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  snfee::io::rhd_sorter::config_type sorter_cfg;
};

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    app_params_type app_params;
    
    // Parse options:
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")

      ("logging,L",
       po::value<std::string>()->value_name("level"),
       "logging priority")

      ("input-list,I",
       po::value<std::string>(&app_params.sorter_cfg.input_rhd_listname)
       ->value_name("path"),
       "set a list of unsorted RHD input filenames")

      ("input-file,i",
       po::value<std::vector<std::string>>(&app_params.sorter_cfg.input_rhd_filenames)
       ->multitoken()
       ->value_name("path"),
       "add an unsorted RHD input file")
       
      ("sort,s", "run in 'sort' mode (default)")
      
      ("sort-buffer-size,S",
       po::value<std::size_t>(& app_params.sorter_cfg.sort_buffer_size)
       ->value_name("number")
       ->default_value(4000),
       "set the size of the sorting buffer of RHD records")
       
      ("evaluation,e", "run in 'evaluation' mode")
      
      ("evaluation-buffer-size,E",
       po::value<std::size_t>(& app_params.sorter_cfg.evaluation_buffer_size)
       ->value_name("number")
       ->default_value(10000),
       "set the size of the evaluation buffer of trigger IDs")
       
      ("no-store,N", "set the 'no store' flag (no output is produced for the 'sort' mode)")

      ("output-list,O",
       po::value<std::string>(&app_params.sorter_cfg.output_rhd_listname)
       ->value_name("path"),
       "set a list of sorted RHD output filenames")

      ("output-file,o",
       po::value<std::vector<std::string>>(&app_params.sorter_cfg.output_rhd_filenames)
       ->value_name("path"),
       "add a sorted RHD output file")

      // ("max-total-records,M",
      //  po::value<std::size_t>(&app_params.sorter_cfg.max_total_records)
      //  ->value_name("number")
      //  ->default_value(0),
      //  "set the maximum number of RHD records to be converted (default: 0, unused")
      
      ("max-records-per-file,f",
       po::value<std::size_t>(& app_params.sorter_cfg.max_records_per_file)
       ->value_name("number")
       ->default_value(0),
       "set the maximum number of collected RHD records per output file")
      
      ("log-modulo,g",
       po::value<std::size_t>(& app_params.sorter_cfg.log_modulo)
       ->value_name("number")
       ->default_value(0),
       "set the size of logging modulo")

      ("report-file,r",
       po::value<std::string>(&app_params.sorter_cfg.report_filename)
       ->value_name("path"),
       "set report filename")
      
    ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
              .options(opts)
              .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "snfee-rhdsort : "
                << "Sort raw hit data file(s) (RHD)"
                << std::endl << std::endl;
      std::cout << "Usage : " << std::endl << std::endl;
      std::cout << "  snfee-rhdsort [OPTIONS]" << std::endl << std::endl;  
      std::cout << opts << std::endl;
      std::cout << "Example : " << std::endl << std::endl;
      std::cout << "  snfee-rhdsort \\\n";
      std::cout << "    --input-file  \"snemo_run-8_urhd_part-0.data.gz\" \\\n";
      std::cout << "    --input-file  \"snemo_run-8_urhd_part-1.data.gz\" \\\n";
      std::cout << "    --input-file  \"snemo_run-8_urhd_part-2.data.gz\" \\\n";
      std::cout << "    --output-file \"snemo_run-8_rhd_part-0.data.gz\" \\\n";
      std::cout << "    --output-file \"snemo_run-8_rhd_part-1.data.gz\" \\\n";
      std::cout << "    --output-file \"snemo_run-8_rhd_part-2.data.gz\" \\\n";
      std::cout << "    --max-records-per-file 100000";
      std::cout << std::endl << std::endl;  
      std::cout << "  snfee-rhdsort \\\n";
      std::cout << "    --input-list \"snemo_run-8_urhd.lis\" \\\n";
      std::cout << "    --output-list \"snemo_run-8_rhd.lis\" \\\n";
      std::cout << "    --max-records-per-file 100000";
      std::cout << std::endl << std::endl;  
      return (-1);
    }

    // Use command line arguments :
    if (vm.count("logging")) {
      std::string logging_repr = vm["logging"].as<std::string>();
      app_params.logging = datatools::logger::get_priority(logging_repr);
      DT_THROW_IF(app_params.logging == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority '" << vm["logging"].as<std::string>() << "'!");
    }
    
    if (vm.count("evaluation")) {
      DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Using 'evaluation' mode");
      app_params.sorter_cfg.mode_label = "evaluation";
    }
    
    if (vm.count("sort")) {
      DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Using 'sort' mode");
      app_params.sorter_cfg.mode_label = "sort";
    }
     
    DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS,
                  "Config mode label = '" << app_params.sorter_cfg.mode_label << "'");
    
    if (vm.count("no-store")) {
      DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Using 'no-store' mode");
      app_params.sorter_cfg.no_store = true;
    }
    
    // Checks:
    DT_THROW_IF(app_params.sorter_cfg.input_rhd_listname.empty() and
                app_params.sorter_cfg.input_rhd_filenames.size() == 0,
                std::logic_error,
                "Missing input RHD filenames!");
    bool with_output = true;
    
    if (app_params.sorter_cfg.mode_label == "evaluation") {
      DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Inhibit output for 'evaluation' mode");
      with_output = false;
    }
    if (! app_params.sorter_cfg.no_store) {
       with_output = false;
    }
    if (with_output) {
      DT_THROW_IF(app_params.sorter_cfg.output_rhd_listname.empty() and
                  app_params.sorter_cfg.output_rhd_filenames.size() == 0,
                  std::logic_error,
                  "Missing output RHD filenames!");
    }
    
    // The RHD sorter:
    snfee::io::rhd_sorter rhdSorter;
    rhdSorter.set_logging(app_params.logging);
    rhdSorter.set_config(app_params.sorter_cfg);

    // Run:
    rhdSorter.initialize();
    rhdSorter.run();
    rhdSorter.terminate();
    
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
