# SNFEE applications

add_subdirectory(CRD2RHD)
if (SNFrontEndElectronics_WITH_CBD2RHD)
  add_subdirectory(CBD2RHD)
endif()
add_subdirectory(RHDSort)
add_subdirectory(RHD2RTD)
# add_subdirectory(RTD2RED)
add_subdirectory(RTD2Root)

# - end