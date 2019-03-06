add_executable(rtd2asroot rtd2asroot.cxx)
target_link_libraries(rtd2asroot PRIVATE SNemoRawDataObj)

add_executable(rtd2brio rtd2brio.cxx)
target_link_libraries(rtd2brio PRIVATE SNemoRawDataObj)
