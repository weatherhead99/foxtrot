add_library(foxtrotclient client.cpp)

target_include_directories(foxtrotclient PUBLIC ${CMAKE_BINARY_DIR}/server/)
target_include_directories(foxtrotclient INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(foxtrotclient INTERFACE foxtrotserv)

add_dependencies(foxtrotclient foxtrotserv)


add_executable(fetchdatatest fetchdatatest.cpp)
target_link_libraries(fetchdatatest foxtrotclient)