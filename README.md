# SNemoRawDataObj
C/C++ Raw Data Structures with Boost/Brio/ROOT I/O for SuperNEMO

# Quickstart
We assume you have a [standard install of the SuperNEMO software](https://github.com/SuperNEMO-DBD/brew).
To build the library and applications:

``` console
$ brew snemo-shell
...
snemo-shell> git clone https://github.com/drbenmorgan/SNemoRawDataObj.git
snemo-shell> cd SNemoRawDataObj
snemo-shell> mkdir build && cd build
snemo-shell> cmake ..
...
snemo-shell> make
...
```

If you encounter errors at the `cmake` or `make` steps, [raise an Issue](https://github.com/drbenmorgan/SNemoRawDataObj.git/issues/new).
On a successful build, you will have the following library and applications in the
directory where you ran `cmake/make`:

- `libSNemoRawData.{so,dylib}`
  - C++ library of Raw Data Classes, plus Boost/ROOT dictionaries
- `crd2rhd`
  - Converts `CRD` raw data streamfiles to `RHD` format streamfiles
- `rhd2rtd`
  - Merges `RHD` streamfiles into offline `RTD` format streamfile
- `rtd2root`
  - Manual conversion of `RTD` streamfiles to a ROOT file containing a TTree
    with branches for different RTD elements
- `rtd2asroot`
  - Automatic, dictionary-based conversion of `RTD` streamfiles to a TTree
    holding an `RTD` branch, plus split branches/leaves for trigger, calo
    and tracker raw data objects
- `rtd2brio`
  - Conversion of `RTD` streamfile to the Brio format understood by the
    [Falaise](https://github.com/SuperNEMO-DBD/Falaise) offline software
    (*NB: currently lacks needed Metadata because `RTD` streamfiles do
    not yet contain this, or a way to obtain it*).

As this project is experimental, it provides no install mechanism, but
all programs, plus interactive ROOT usage, can be run from the directory
holding the above programs.

# Usage for Analysis
1. Convert an `RTD` file to full ROOT:

   ``` console
   snemo-shell> ./rtd2asroot -i /sps/nemo/snemo/snemo_data/raw_data/RTD/run_102/snemo_run-102_rtd.data.gz -o run_102.root
   ...
   snemo-shell> root
   ...
   root [0] TFile f("run_102.root")
   ... will see Cling errors here, but can be ignored for now...
   root [1] f.ls()
   TFile**		/tmp/test.root
   TFile*		/tmp/test.root
   KEY: TTree	RawTriggerData;13	SuperNEMO Raw Trigger Data
   KEY: TTree	RawTriggerData;12	SuperNEMO Raw Trigger Data
   ```

   Note that there are two namecycles here due to the large amount of data
   and ROOT's bookkeeping. No excess data is stored though, and use of the
   `RawTriggerData` TTree is transparent:

   root [2] TTree* t = (TTree*)f.Get("RawTriggerData")
   (TTree *) 0x377ff80
   root [3] t->GetEntries()
   (long long) 282801
   root [4] t->Print()
   ... long list of branches, their types, and stats
   ```

   Using the above list, and the code as documented in ..., you can
   easily write your own ROOT scripts to analyse the `RawTriggerData` TTree.

2. Convert an `RTD` file to Brio:

   ``` console
   snemo-shell> ./rtd2brio -i /sps/nemo/snemo/snemo_data/raw_data/RTD/run_102/snemo_run-102_rtd.data.gz -o run_102.brio
   snemo-shell> flreconstruct -i run_102.brio
   ... fails for now ...
   ```

   The failure is due to a lack of Metadata, so need work and discussion
   to find out where this will come from.



# Structure
- `vendor_snfee`
  - All code from full SNFrontEndElectronics from data/io parts, plus code needed
    to close `#include` dependencies
- `snfee/{data,io,model}
  - Code from `vendor_snfee` with following changes:
    - Dependency on `SNCabling` in `snfee/data/channel_id.h` commented out.
      This code just does a copy conversion, and should be handled via a free
      function, or simpler object like a `std::tuple`.
  - Code obviously relating to *analysis*, *presentation* is moved to
    `elsewhere` directory.
    - Similarly anything that appears unrelated to raw data structures
  - Code obviously relating to specific I/O conversions like CRD2RHD or
    RHD2RTD is moved into directories for these applications.
    - See `programs/{crd2rhd,rhd2rtd,rtd2root}`

Note that `snfee` still contains code which *may* be analysis/conditions
related:

- `calo_channel_traits.{h,cc}`
- `calo_signal_model.{h,cc}`
- `calo_waveform_data.{h,cc}`

These have serialization methods, but do not seem used by anything directly
related to CRD/RHD/RTD data.
