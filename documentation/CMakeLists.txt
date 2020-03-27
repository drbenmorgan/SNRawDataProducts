# Minimal Doxygen build (requires CMake 3.9 or newer)
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_EXCLUDE_PATTERNS "*.cc" "${PROJECT_BINARY_DIR}/*")
  set(DOXYGEN_STRIP_FROM_INC_PATH "${PROJECT_SOURCE_DIR}")
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
  set(DOXYGEN_GENERATE_TREEVIEW "YES")

  doxygen_add_docs(snfee-doxygen
    README.md snfee
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Doxygenating SNRawDataProducts"
  )
endif()