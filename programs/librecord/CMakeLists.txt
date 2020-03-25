# Private library for buffers between the RHD -> RTD -> RED stages
# These are only used in building each stage by the converion/builder
# applications, so do not appear to be required in the public interface(?)
add_library(librecord STATIC
  red_record.cc
  red_record.h
  rhd_record.cc
  rhd_record.h
  rtd_record.cc
  rtd_record.h
)
target_include_directories(librecord PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(librecord PUBLIC SNRawDataProducts)