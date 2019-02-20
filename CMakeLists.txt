cmake_minimum_required(VERSION 3.3)
project(SNemoRawDataObj VERSION 0.1.0)

# Direct Dependencies
find_package(Bayeux 3.4.1 REQUIRED)

# Bayeux will refind Boost, but we need an extra component
find_package(Boost REQUIRED date_time)

# Need ROOT for dictionary generation
find_package(ROOT 6.12 REQUIRED)

# The library
add_library(SNemoRawDataObj SHARED
  snfee/data/calo_channel_traits.cc
  snfee/data/calo_channel_traits.h
  snfee/data/calo_hit_record.cc
  snfee/data/calo_hit_record.h
  snfee/data/calo_signal_model.cc
  snfee/data/calo_signal_model.h
  snfee/data/calo_waveform_data.cc
  snfee/data/calo_waveform_data.h
  snfee/data/channel_id.cc
  snfee/data/channel_id.h
  snfee/data/channel_id_selection.cc
  snfee/data/channel_id_selection.h
  snfee/data/has_trigger_id_interface.h
  snfee/data/raw_trigger_data.cc
  snfee/data/raw_trigger_data.h
  snfee/data/rtd2root_data.cc
  snfee/data/rtd2root_data.h
  snfee/data/rtd_selection.cc
  snfee/data/rtd_selection.h
  snfee/data/run_info-serial.h
  snfee/data/run_info.cc
  snfee/data/run_info.h
  snfee/data/tracker_hit_record.cc
  snfee/data/tracker_hit_record.h
  snfee/data/trigger_record.cc
  snfee/data/trigger_record.h
  snfee/data/utils.h
  snfee/geometry.h
  snfee/model/feb_constants.cc
  snfee/model/feb_constants.h
  snfee/model/utils.cc
  snfee/model/utils.h
  snfee/utils.cc
  snfee/utils.h

  snfee/io/calo_hit_parser.cc
  snfee/io/calo_hit_parser.h
  snfee/io/multifile_data_reader.cc
  snfee/io/multifile_data_reader.h
  snfee/io/multifile_data_writer.cc
  snfee/io/multifile_data_writer.h
  snfee/io/raw_hit_reader.cc
  snfee/io/raw_hit_reader.h
  snfee/io/raw_record_parser.cc
  snfee/io/raw_record_parser.h
  snfee/io/raw_run_header.cc
  snfee/io/raw_run_header.h
  snfee/io/rhd_record.cc
  snfee/io/rhd_record.h
  snfee/io/rhd_sorter.cc
  snfee/io/rhd_sorter.h
  # # Better handled later (Create dicts for types, plus we need ROOT)
  #snfee/io/rtd2root_converter.cc
  #snfee/io/rtd2root_converter.h
  snfee/io/rtd_record.cc
  snfee/io/rtd_record.h
  snfee/io/tracker_hit_parser.cc
  snfee/io/tracker_hit_parser.h
  )

target_include_directories(SNemoRawDataObj PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  )
target_link_libraries(SNemoRawDataObj PUBLIC Bayeux::Bayeux Boost::date_time)

# Now the Boost.serialization dictionary
# Not compilable yet due to mygsl tabulated function interface change
add_library(SNemoRawDataObj_bdict SHARED snfee/boost_dict.cc)
target_link_libraries(SNemoRawDataObj_bdict PRIVATE SNemoRawDataObj)

# Now the Root dictionary
#include(${ROOT_DIR}/modules/RootNewMacros.cmake)
# need at least one inc dir
#include_directories(${PROJECT_SOURCE_DIR})
#include_directories(${Boost_INCLUDE_DIRS})
#root_generate_dictionary(SNemoRawDataObj_dict
  # Don't add headers explicitly or their buildtime paths end up in the dict
  # Use #include in the linkdef to bring in what's needed.
  #  MODULE SNemoRawDataObj
  # LINKDEF ${PROJECT_SOURCE_DIR}/snfee/root_dict_linkdef.h
  #OPTIONS "-noIncludePaths"
  #)
#add_library(SNemoRawDataObj_rdictlib SHARED SNemoRawDataObj_dict.cxx)
#target_link_libraries(SNemoRawDataObj_rdictlib PUBLIC SNemoRawDataObj)

