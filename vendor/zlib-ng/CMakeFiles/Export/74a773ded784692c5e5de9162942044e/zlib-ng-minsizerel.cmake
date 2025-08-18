#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "zlib-ng::zlib" for configuration "MinSizeRel"
set_property(TARGET zlib-ng::zlib APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(zlib-ng::zlib PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/zlib-ng.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/zlib-ng2.dll"
  )

list(APPEND _cmake_import_check_targets zlib-ng::zlib )
list(APPEND _cmake_import_check_files_for_zlib-ng::zlib "${_IMPORT_PREFIX}/lib/zlib-ng.lib" "${_IMPORT_PREFIX}/bin/zlib-ng2.dll" )

# Import target "zlib-ng::zlibstatic" for configuration "MinSizeRel"
set_property(TARGET zlib-ng::zlibstatic APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(zlib-ng::zlibstatic PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "C"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/zlibstatic-ng.lib"
  )

list(APPEND _cmake_import_check_targets zlib-ng::zlibstatic )
list(APPEND _cmake_import_check_files_for_zlib-ng::zlibstatic "${_IMPORT_PREFIX}/lib/zlibstatic-ng.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
