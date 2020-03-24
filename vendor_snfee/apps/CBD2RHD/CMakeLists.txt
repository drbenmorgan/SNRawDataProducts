
# message(STATUS "[debug] ========= PROJECT_BINARY_DIR = '${PROJECT_BINARY_DIR}'")
# message(STATUS "[debug] ========= PROJECT_SOURCE_DIR = '${PROJECT_SOURCE_DIR}'")
include_directories(${PROJECT_BINARY_DIR}/../SNComDAQ)
include_directories(${PROJECT_SOURCE_DIR}/../SNComDAQ/source)

add_executable(snfee-cbd2rhd cbd2rhd.cxx 
  calo_convertor.h
  tracker_convertor.h
  trigger_convertor.h
  calo_convertor.cc
  tracker_convertor.cc
  trigger_convertor.cc
  )
target_link_libraries(snfee-cbd2rhd
  snfee
  sncomdaqc
  Bayeux::Bayeux
  )
set_target_properties(snfee-cbd2rhd
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  )
install(FILES
  ${CMAKE_BINARY_DIR}/snfee-cbd2rhd 
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# end