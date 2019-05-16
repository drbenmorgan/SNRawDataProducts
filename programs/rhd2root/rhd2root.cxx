//! Convert an RHD Stream to Root

// - Boost:
#include <boost/program_options.hpp>

#include <iostream>

#include "snfee/data/calo_hit_record.h"
#include "snfee/io/multifile_data_reader.h"

// Input is 1-N RTD file to convert
// Output is 1 root file
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeIndex.h"

#include "ROOT/TSeq.hxx"
#include "TRandom.h"

namespace {
  // Quick and dumb mess-up of trigger ids
  int32_t mixTriggerID(int32_t t) {
    int32_t nt = t + static_cast<int32_t>(gRandom->Gaus(0,10));
    return nt < 0 ? 0 : nt;
  }
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
      "path to RHD input file")
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
  // 1. Determine type of input RHD stream (calo, tracker, trigger)
  {
    snfee::io::multifile_data_reader reader{inputConfig};
    std::string rhdType{reader.get_record_tag()};
    std::clog << "Input stream is type '" << rhdType << "'" << std::endl;;
  }

  // Assume stream is pure
  // Will have a function to pass reader.get_record_tag() to
  using RHDType = snfee::data::calo_hit_record;

  // 2. Convert
  {
    // RHD Boost stream
    snfee::io::multifile_data_reader reader{inputConfig};

    // ROOT output
    TFile writer{outputFile.c_str(), "RECREATE"};
    TTree rhdTree{"RawHitData", "SuperNEMO Raw Hit Data"};

    auto rhd = new RHDType{};
    rhdTree.Branch("RHD", &rhd);

    size_t counter{0};
    std::string msg {"Converting RHD stream to ROOT Tree, done: "};
    while(reader.has_record_tag()) {
      reader.load(*rhd);

      // Mix up to model non-time ordering
      rhd->set_trigger_id(mixTriggerID(rhd->get_trigger_id()));

      rhdTree.Fill();

      if (! (counter % 1000)) std::clog << msg << counter << "\r";
      counter++;
    }
    std::clog << msg << counter << "\n";

    // Can print/output tree stats here if required
    //rhdTree.Print("RHD");
    writer.Write();
  }

  // 3. Sort by trigger_id
  {
    ROOT::EnableImplicitMT();

    TFile oldFile{outputFile.c_str()};
    TTree* oldTree;
    oldFile.GetObject("RawHitData", oldTree);
    // Help random access performance, can expect same trigger_ids to
    // be close to each other
    // To be adjusted based on typical RHD file size
    oldTree->SetMaxVirtualSize(1e9);

    const auto nentries = oldTree->BuildIndex("_trigger_id_");
    TTreeIndex* index = (TTreeIndex*)oldTree->GetTreeIndex();

    RHDType* hit{nullptr};
    oldTree->SetBranchAddress("RHD", &hit);

    TFile newFile{"sorted.root", "RECREATE"};
    auto newTree = oldTree->CloneTree(0);

    std::string msg{"Sorting RHD Tree by Trigger ID: "};

    for (auto i : ROOT::TSeqI(nentries)) {
      oldTree->GetEntry(index->GetIndex()[i]);
      newTree->Fill();
      if (! (i % 1000)) std::clog << msg << 100*i/nentries << "%\r";
    }
    std::clog << msg << "100%\n";

    // Can print/output tree stats here if required
    // newTree->Print();
    newFile.Write();
  }

  return 0;
}
