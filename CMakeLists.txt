cmake_minimum_required(VERSION 3.3)
project(SNemoRawDataObj VERSION 0.1.0)

# Direct Dependencies
find_package(Bayeux 3.3.0 REQUIRED)
# A hack until
# - root_generate_dictionary support usage requirements/genexs
# - Bayeux sets a variable for its include path
if(NOT Bayeux_INCLUDE_DIRS)
  # NB assumes a normal install
  find_path(Bayeux_INCLUDE_DIRS bayeux/bayeux.h
    PATHS "${Bayeux_DIR}/../../../"
    PATH_SUFFIXES include
    NO_DEFAULT_PATH)
endif()
if(Bayeux_INCLUDE_DIRS)
    list(APPEND Bayeux_INCLUDE_DIRS "${Bayeux_INCLUDE_DIRS}/bayeux")
endif()

# Bayeux will refind Boost, but we need an extra component
find_package(Boost REQUIRED date_time)

# Need ROOT for dictionary generation
find_package(ROOT 6.12 REQUIRED)

# Need threads for the rhd2rtd program
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# - Library build
# Add the Boost/Root dictionaries into the library for now
include(${ROOT_DIR}/modules/RootNewMacros.cmake)
# Need incdirs until root_generate_dictionary support usage requirements/genexs
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${Bayeux_INCLUDE_DIRS})
root_generate_dictionary(SNemoRawDataObj_dict
  # Don't add headers explicitly or their buildtime paths end up in the dict
  # Use #include in the linkdef to bring in what's needed.
  MODULE SNemoRawDataObj
  LINKDEF ${PROJECT_SOURCE_DIR}/snfee/root_dict_linkdef.h
  OPTIONS "-noIncludePaths")

# The library itself
add_library(SNemoRawDataObj SHARED
  # Data Model
  snfee/data/calo_hit_record.cc
  snfee/data/calo_hit_record.h
  snfee/data/channel_id.cc
  snfee/data/channel_id.h
  snfee/data/has_trigger_id_interface.h
  snfee/data/raw_trigger_data.cc
  snfee/data/raw_trigger_data.h
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
  # Boost.Serialization File Reader/Writers
  snfee/io/multifile_data_reader.cc
  snfee/io/multifile_data_reader.h
  snfee/io/multifile_data_writer.cc
  snfee/io/multifile_data_writer.h
  # Boost.Serialization, Root dictionaries
  snfee/boost_dict.cc
  ${CMAKE_CURRENT_BINARY_DIR}/SNemoRawDataObj_dict.cxx
  )

target_include_directories(SNemoRawDataObj PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  )
target_link_libraries(SNemoRawDataObj PUBLIC Bayeux::Bayeux Boost::date_time ROOT::RIO)

# Programs
add_subdirectory(programs)

# Examples
add_subdirectory(examples)

