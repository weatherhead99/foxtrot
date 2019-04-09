find_library(libusb NAMES usb-1.0 libusb-1.0 usb)
find_path(libusb_include NAMES libusb.h usb.h PATH_SUFFIXES libusb-1.0)
if(libusb)
    add_device_object_lib(stellarnet stellarnet.cpp stellarnet.h)
    target_include_directories(stellarnet PUBLIC ${libusb_include})
endif()


# add_executable(stellarnet_play stellarnet_play.cpp)
# target_link_libraries(stellarnet_play stellarnet foxtrot foxtrotclient ${libusb})
