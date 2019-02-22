//! Try and improve RTD in ROOT using full dictionaries

// - Boost:
#include <boost/program_options.hpp>

#include <iostream>


// Input is 1 RTD file to convert
// Output is 1 root file

int main(int argc, char *argv[])
{
  std::string inputFile{};
  std::string outputFile{};

  namespace po = boost::program_options;
  po::options_description opts("Allowed options");
  opts.add_options()
    ("help,h", "produce help message")
    ("input-file,i",
      po::value<std::string>(&inputFile)
      ->value_name("<PATH>")
      ->required(),
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

  std::cout << inputFile << ", " << outputFile << "\n";

  // Open input file, expect rtd format, so
  // use snfee::io::multifile_data_reader
  // pseudo setup is:
  //
  // snfee::data::raw_trigger_data rtdAsBoost;
  //
  // while(reader->has_record_tag()) {
  //   if(reader->record_tag_is(snfee::data::raw_trigger_data::SERIAL_TAG)) {
  //     reader->load(rtdAsBoost)
  //
  //     ... do stuff with rtdAsBoost ...
  //
  //   }
  // }
  //

  return 0;
}
