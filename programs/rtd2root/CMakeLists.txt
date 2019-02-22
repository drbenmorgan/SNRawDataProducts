add_executable(rtd2root rtd2root.cxx
  rtd2root_data.cc
  rtd2root_data.h
  rtd2root_converter.cc
  rtd2root_converter.h
  )
target_link_libraries(rtd2root PRIVATE SNemoRawDataObj)

