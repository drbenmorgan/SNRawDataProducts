
add_executable(snfee-rtd2red rtd2red.cxx)

target_link_libraries(snfee-rtd2red
  snfee
  Bayeux::Bayeux
  )
set_target_properties(snfee-rtd2red
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  )
install(FILES
    ${CMAKE_BINARY_DIR}/snfee-rtd2red 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    )
