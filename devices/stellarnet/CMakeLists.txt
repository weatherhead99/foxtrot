find_package(libusb)


if(libusb_FOUND)
  message(NOTIFY "found libUSB, building stellarnet driver")
  add_library(stellarnet OBJECT stellarnet.cpp)
  target_sources(stellarnet PUBLIC FILE_SET HEADERS FILES stellarnet.h)
  target_link_libraries(stellarnet PUBLIC libusb::libusb)
  target_link_libraries(foxtrot_devices INTERFACE libusb::libusb)
  ft_device_lib_configure(stellarnet)
  
else()
    message(WARNING "couldn't find libusb, not building stellarnet driver")
endif()


add_executable(stellarnet_play stellarnet_play.cpp)
target_link_libraries(stellarnet_play PUBLIC stellarnet)
