include(FindPackageHandleStandardArgs)


find_package(PkgConfig REQUIRED)
pkg_check_modules(libudev REQUIRED libudev IMPORTED_TARGET)

if(${libudev_FOUND})
  message(STATUS "found libudev, include directory: ${libudev_INCLUDE_DIRS}")
  message(STATUS "libraries: ${libudev_LIBRARIES}")

  if(NOT TARGET libudev::libudev)
    add_library(libudev::libudev ALIAS PkgConfig::libudev)
  endif()
endif()




find_package_handle_standard_args(libudev REQUIRED_VARS libudev_FOUND
VERSION_VAR libudev_VERSION)

