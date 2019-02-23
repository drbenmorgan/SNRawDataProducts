# SNemoRawDataObj
C/C++ Raw Data Structures and I/O for SuperNEMO

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
