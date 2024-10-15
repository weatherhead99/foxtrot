include(FindPackageHandleStandardArgs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libsodium REQUIRED libsodium IMPORTED_TARGET)

if(${libsodium_FOUND})
  message(STATUS "found libsodium, include directory ${libsodium_INCLUDE_DIRS}")
  message(STATUS "libraries: ${libsodium_LIBRARIES}")
  if(NOT TARGET libsodium::libsodium)
    add_library(libsodium::libsodium ALIAS PkgConfig::libsodium)
  endif()
endif()

find_package_handle_standard_args(libsodium REQUIRED_VARS libsodium_FOUND
  VERSION_VAR libsodium_VERSION)
