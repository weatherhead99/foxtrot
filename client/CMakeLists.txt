add_library(foxtrotclient client.cpp)

target_include_directories(foxtrotclient PUBLIC ${CMAKE_BINARY_DIR}/server/)
target_include_directories(foxtrotclient INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(foxtrotclient foxtrotserv)

