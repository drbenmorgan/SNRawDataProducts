//! Try and improve RTD in ROOT using full dictionaries

// - Boost:
#include <boost/program_options.hpp>

#include <iostream>

#include "snfee/data/raw_trigger_data.h"
#include "snfee/data/RRawTriggerData.h"
#include "snfee/io/multifile_data_reader.h"

// Input is 1-N RTD file to convert
// Output is 1 root file
#include "TFile.h"
#include "TTree.h"

//! Convert "raw raw" data to standard value type form
snfee::data::RRawTriggerData
convertRTD(const snfee::data::raw_trigger_data& rawRTD)
{
  int32_t runID {rawRTD.get_run_id()};
  int32_t triggerID {rawRTD.get_trigger_id()};

  snfee::data::TriggerRecord triggerRec{};
  if (rawRTD.has_trig()) {
    triggerRec = *(rawRTD.get_trig());
  }

  const auto& inputCaloRec = rawRTD.get_calo_hits();
  snfee::data::CaloRecordCollection outputCaloRec{};

  for(const auto& crec : inputCaloRec) {
    outputCaloRec.push_back(*crec);
  }

  const auto& inputTrackerRec = rawRTD.get_tracker_hits();
  snfee::data::TrackerRecordCollection outputTrackerRec;

  for(const auto& crec : inputTrackerRec) {
    outputTrackerRec.push_back(*crec);
  }

  return snfee::data::RRawTriggerData {runID, triggerID, triggerRec, outputCaloRec, outputTrackerRec};
}


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
  TFile writer {outputFile.c_str(), "RECREATE"};
  TTree rtdTree {"RawTriggerData", "SuperNEMO Raw Trigger Data"};

  snfee::data::RRawTriggerData* workingRTD {nullptr};
  rtdTree.Branch("RTD", &workingRTD);

  // Test handful of records only
  size_t counter{0};
  size_t maxRecords{5};

  while(reader.has_record_tag() && reader.record_tag_is(snfee::data::raw_trigger_data::SERIAL_TAG)) {
    reader.load(rtdRaw);
    *workingRTD = convertRTD(rtdRaw);
    rtdTree.Fill();

    if (! (counter % 1000)) std::clog << "Processed record: " << counter << "\n";
    counter++;
  }

  std::clog << "Total records processed: " << counter << "\n";
  std::clog << "Following TTree Written:\n";

  rtdTree.Print();
  writer.Write();

  return 0;
}
