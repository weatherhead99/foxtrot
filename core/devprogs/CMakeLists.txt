
add_executable(tuple_play tuple_play.cpp)
target_link_libraries(tuple_play PUBLIC protobuf::libprotobuf foxtrot_core)
target_include_directories(tuple_play PRIVATE ${CMAKE_BINARY_DIR})


add_executable(byte_convert_play byte_convert_play.cpp)
target_link_libraries(byte_convert_play PUBLIC foxtrot_core)
target_include_directories(byte_convert_play PUBLIC ${CMAKE_BINARY_DIR})

add_executable(arith_test arith_test.cpp)
target_link_libraries(arith_test PUBLIC foxtrot_core)
target_include_directories(arith_test PUBLIC ${CMAKE_BINARY_DIR})


add_executable(union_play union_play.cpp)
target_link_libraries(union_play PUBLIC foxtrot_core)
target_include_directories(union_play PUBLIC ${CMAKE_BINARY_DIR})

add_executable(timestamp_play timestamp_play.cpp)
target_link_libraries(timestamp_play foxtrot_core)
target_include_directories(timestamp_play PUBLIC ${CMAKE_SOURCE_DIR}/include)


add_executable(plugin_play plugin_play.cpp)
target_link_libraries(plugin_play Boost::headers)
add_library(plugin_example SHARED plugin_example.cpp)
