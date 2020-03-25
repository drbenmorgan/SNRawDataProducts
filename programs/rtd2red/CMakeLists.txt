add_executable(rtd2red rtd2red.cxx
  builder.cc
  builder.h
  builder_config.cc
  builder_config.h
  builder_pimpl.h
  input_worker.cc
  input_worker.h
  output_worker.cc
  output_worker.h
  red_buffer.cc
  red_buffer.h
  rtd2red_builder.cc
  rtd2red_builder.h
  rtd_buffer.cc
  rtd_buffer.h
)
target_link_libraries(rtd2red PRIVATE librecord SNRawDataProducts Threads::Threads)
_snrtd_install_rpath(rtd2red)

install(TARGETS rtd2red EXPORT SNRawDataProductsTargets DESTINATION ${CMAKE_INSTALL_BINDIR})
