add_device_object_lib(NAME RLY_8
  SRCS RLY_8.cpp
  INCLUDES RLY_8.hh)


add_executable(rly_8_play rly_8_play.cpp)
target_link_libraries(rly_8_play foxtrot::foxtrot_protocols RLY_8)
