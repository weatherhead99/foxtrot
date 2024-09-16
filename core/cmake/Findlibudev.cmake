include(FindPkgConfig)
include(FindPackageHandleStandardArgs)


pkg_check_modules(libudev REQUIRED libudev IMPORTED_TARGET)

if(${libudev_FOUND})
  message(STATUS "found libudev, include directory: ${libudev_INCLUDE_DIRS}")
  message(STATUS "libraries: ${libudev_LIBRARIES}")

  if(NOT TARGET udev::libudev)
    add_library(udev::libudev ALIAS PkgConfig::libudev)
  endif()
endif()




find_package_handle_standard_args(libudev DEFAULT_MSG libudev_INCLUDE_DIRS libudev_LIBRARIES)

