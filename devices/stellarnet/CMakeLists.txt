find_package(libusb)


if(${libusb_FOUND})
    add_device_object_lib(NAME stellarnet SRCS stellarnet.cpp INCLUDES stellarnet.h
    LINKLIBS libusb::libusb)
else()
    message(WARNING "couldn't find libusb, not building stellarnet driver")
endif()


add_executable(stellarnet_play stellarnet_play.cpp)
target_link_libraries(stellarnet_play stellarnet foxtrot::foxtrot_core )
