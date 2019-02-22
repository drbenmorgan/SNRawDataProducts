# Build programs
add_subdirectory(crd2rhd)

add_executable(rhd2rtd rhd2rtd.cxx)
target_link_libraries(rhd2rtd PRIVATE SNemoRawDataObj)

add_subdirectory(rtd2root)
