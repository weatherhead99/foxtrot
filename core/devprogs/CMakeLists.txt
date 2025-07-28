
add_executable(tuple_play tuple_play.cpp)
target_link_libraries(tuple_play PUBLIC foxtrot_core)

add_executable(byte_convert_play byte_convert_play.cpp)
target_link_libraries(byte_convert_play PUBLIC foxtrot_core)

add_executable(arith_test arith_test.cpp)
target_link_libraries(arith_test PUBLIC foxtrot_core)

add_executable(union_play union_play.cpp)
target_link_libraries(union_play PUBLIC foxtrot_core)

add_executable(timestamp_play timestamp_play.cpp)
target_link_libraries(timestamp_play PUBLIC foxtrot_core)

add_executable(plugin_play plugin_play.cpp)
target_link_libraries(plugin_play Boost::headers)
add_library(plugin_example SHARED plugin_example.cpp)

message(STATUS "protobuf libs: ${PROTOBUF_LIBRARIES}")


if(BUILD_UDEV_SUPPORT)
  add_executable(udev_play udev_play.cpp)
  target_link_libraries(udev_play foxtrot_core)

  add_executable(m30x_stage_discover_play m30x_stage_discover_play.cpp)
  target_link_libraries(m30x_stage_discover_play foxtrot_core)
  
endif()
