add_library(foxtrot_udev udev_wrapper.cpp)
add_executable(udev_play udev_play.cpp)

#TODO: find udev library
target_link_libraries(udev_play foxtrot_udev -ludev)
