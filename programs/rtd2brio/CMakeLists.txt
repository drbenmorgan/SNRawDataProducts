add_executable(rtd2brio rtd2brio.cxx)
target_link_libraries(rtd2brio PRIVATE SNemoRawDataObj Boost::program_options)
_snrtd_install_rpath(rtd2brio)

install(TARGETS rtd2brio EXPORT SNemoRawDataObjTargets DESTINATION ${CMAKE_INSTALL_BINDIR})

