# Build programs

add_executable(crd2rhd crd2rhd.cxx)
target_link_libraries(crd2rhd PUBLIC SNemoRawDataObj)

add_executable(rhd2rtd rhd2rtd.cxx)
target_link_libraries(rhd2rtd PRIVATE SNemoRawDataObj)

add_subdirectory(rtd2root)
