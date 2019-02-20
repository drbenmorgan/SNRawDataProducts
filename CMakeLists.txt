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
  )

target_include_directories(SNemoRawDataObj PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  )
target_link_libraries(SNemoRawDataObj PUBLIC Bayeux::Bayeux Boost::date_time)

# Now the Boost.serialization dictionary
# Not compilable yet due to mygsl tabulated function interface change
#add_library(SNemoDataObj_boost_dict SHARED snfee/boost_dict.cc)
#target_link_libraries(SNemoDataObj_boost_dict PRIVATE SNemoDataObj)

# Now the Root dictionary
#include(${ROOT_DIR}/modules/RootNewMacros.cmake)
# need at least one inc dir
#include_directories(${PROJECT_SOURCE_DIR})
#include_directories(${Boost_INCLUDE_DIRS})
#include_directories(${Boost_INCLUDE_DIRS}/bayeux)
#root_generate_dictionary(SNemoDataObj_rdict
  # Don't add headers explicitly or their buildtime paths end up in the dict
  # Use #include in the linkdef to bring in what's needed.
  #  MODULE SNemoRawDataObj
  #  LINKDEF ${PROJECT_SOURCE_DIR}/snfee/root_dict_linkdef.h
  #  OPTIONS "-noIncludePaths"
  #  )
#
