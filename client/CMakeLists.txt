add_library(foxtrotclient client.cpp)

target_include_directories(foxtrotclient PRIVATE ${CMAKE_BINARY_DIR}/server/)
