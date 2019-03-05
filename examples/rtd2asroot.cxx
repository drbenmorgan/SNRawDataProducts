//! Try and improve RTD in ROOT using full dictionaries

// - Boost:
#include <boost/program_options.hpp>

#include <iostream>

#include "snfee/data/raw_trigger_data.h"
#include "snfee/io/multifile_data_reader.h"

// Input is 1-N RTD file to convert
// Output is 1 root file

int main(int argc, char *argv[])
{
  // - Command Line
  snfee::io::multifile_data_reader::config_type inputConfig;
  std::string outputFile{};

  namespace po = boost::program_options;
  po::options_description opts("Allowed options");
  opts.add_options()
    ("help,h", "produce help message")
    ("input-file,i",
      po::value<std::vector<std::string>>(&inputConfig.filenames)
      ->value_name("<PATH>")
      ->required()
      ->multitoken(),
      "path to RTD input file")
    ("output-file,o",
      po::value<std::string>(&outputFile)
      ->value_name("<PATH>")
      ->required(),
      "path to ROOT output file");

  // Describe command line arguments
  try {
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
              .options(opts)
              .run(), vm);
    if (vm.count("help")) {
      std::cout << opts << "\n";
      return 0;
    }
    po::notify(vm);
  }
  catch (std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return EXIT_FAILURE;
  }

  // - Processing
  // Input
  snfee::data::raw_trigger_data rtdRaw;
  snfee::io::multifile_data_reader reader{inputConfig};

  // Output
  // Create TFile, TTree, branches?

  // Test handful of records only
  size_t counter{0};
  size_t maxRecords{5};

  while(reader.has_record_tag() && reader.record_tag_is(snfee::data::raw_trigger_data::SERIAL_TAG) && counter < 5) {
    reader.load(rtdRaw);
    rtdRaw.tree_dump();

    // Reserialize?
    // Convert to non-shared-ptr form Fill tree?
    counter++;
  }


  return 0;
}
