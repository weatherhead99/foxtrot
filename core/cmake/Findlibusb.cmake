
find_path(LIBUSB_INCLUDE_DIR NAMES libusb.h PATH_SUFFIXES libusb-1.0)
find_library(LIBUSB_LIBRARY NAMES usb-1.0 usb)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libusb DEFAULT_MSG LIBUSB_INCLUDE_DIR LIBUSB_LIBRARY)

if(TARGET libusb::libusb)
    message(STATUS "libusb target already exists, not doing anything")
else()
    add_library(libusb::libusb UNKNOWN IMPORTED GLOBAL)
    set_target_properties(libusb::libusb PROPERTIES IMPORTED_LOCATION ${LIBUSB_LIBRARY})
    set_target_properties(libusb::libusb PROPERTIES 
                            INTERFACE_INCLUDE_DIRECTORIES ${LIBUSB_INCLUDE_DIR})
endif()
