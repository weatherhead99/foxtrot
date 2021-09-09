if(NOT WIN32)
#some weird linker error on windows platforms
add_executable(tuple_play tuple_play.cpp)
target_link_libraries(tuple_play PUBLIC foxtrot_core)
target_include_directories(tuple_play PRIVATE ${CMAKE_BINARY_DIR})
endif()
