if(libusb_FOUND)
  add_executable(libusb_play libusb_play.cpp)
  target_link_libraries(libusb_play PUBLIC foxtrot_protocols)
endif()


add_executable(simpletcp_asio_play simpletcp_asio_play.cpp)
target_link_libraries(simpletcp_asio_play PUBLIC foxtrot_protocols)

#add_executable(udev_play udev_play.cpp)

