add_library(foxtrotclient client.cpp)

target_include_directories(foxtrotclient PUBLIC ${CMAKE_BINARY_DIR}/server/)
target_include_directories(foxtrotclient INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(foxtrotclient INTERFACE foxtrotserv)

add_dependencies(foxtrotclient foxtrotserv)

install(TARGETS foxtrotclient LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot)

file(GLOB ft_client_includes *.h)
install(FILES ${ft_client_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot)

add_executable(fetchdatatest fetchdatatest.cpp)
target_link_libraries(fetchdatatest foxtrotclient)
