add_device_lib(DM3068 DM3068.cpp)


add_executable(DM3068_play DM3068_play.cpp)
target_link_libraries(DM3068_play foxtrot DM3068)
