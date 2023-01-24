if(NOT WIN32)
#some weird linker error on windows platforms
add_executable(tuple_play tuple_play.cpp)
target_link_libraries(tuple_play PUBLIC foxtrot_core)
target_include_directories(tuple_play PRIVATE ${CMAKE_BINARY_DIR})
endif()


add_executable(byte_convert_play byte_convert_play.cpp)
target_link_libraries(byte_convert_play PUBLIC RTTR::Core foxtrot_core)
target_include_directories(byte_convert_play PUBLIC ${CMAKE_BINARY_DIR})

