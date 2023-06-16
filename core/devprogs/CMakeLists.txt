add_executable(tuple_play tuple_play.cpp)
target_link_libraries(tuple_play PUBLIC foxtrot_core RTTR::Core)
target_include_directories(tuple_play PRIVATE ${CMAKE_BINARY_DIR})


add_executable(byte_convert_play byte_convert_play.cpp)
target_link_libraries(byte_convert_play PUBLIC foxtrot_core RTTR::Core)
target_include_directories(byte_convert_play PUBLIC ${CMAKE_BINARY_DIR})

add_executable(arith_test arith_test.cpp)
target_link_libraries(arith_test PUBLIC foxtrot_core RTTR::Core)
