// - Boost:
#include <boost/program_options.hpp>

#include <iostream>
#include <vector>

#include "snfee/data/raw_trigger_data.h"
#include "snfee/io/multifile_data_reader.h"
#include "bayeux/brio/writer.h"
#include "bayeux/datatools/things.h"

int main(int argc, const char *argv[])
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
      ->required()
      ->multitoken()
      ->value_name("<PATH>"),
      "path to RTD input file")
    ("output-file,o",
      po::value<std::string>(&outputFile)
      ->required()
      ->value_name("<PATH>"),
      "path to Brio output file");

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
  snfee::data::raw_trigger_data rtdRaw;
  snfee::io::multifile_data_reader reader{inputConfig};

  // Brio writer will have GI and ER stores, RTD data
  // will be put in "RTD" bank in ER store
  brio::writer writer{outputFile};
  writer.add_store("GI","datatools::properties");
  writer.add_store("ER","datatools::things");
  writer.tree_dump();

  while(reader.has_record_tag() && reader.record_tag_is(snfee::data::raw_trigger_data::SERIAL_TAG)) {
    //datatools::things workItem;
    //auto rtdRaw = workItem.add<snfee::data::raw_trigger_data>("RTD");
    snfee::data::raw_trigger_data rtdRawTMP;
    reader.load(rtdRawTMP);
    //writer.store(workItem,"ER");
  }

  return 0;
}
