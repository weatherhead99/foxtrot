add_library(foxtrotclient client.cpp)
target_compile_definitions(foxtrotclient PRIVATE -DPROTOBUF_VERSION=${Protobuf_VERSION})

target_include_directories(foxtrotclient PUBLIC ${CMAKE_BINARY_DIR}/server/)
target_include_directories(foxtrotclient INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(foxtrotclient INTERFACE foxtrotserv ${EXTRA_WINLIBS})

add_dependencies(foxtrotclient foxtrotserv)

install(TARGETS foxtrotclient LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot)

file(GLOB ft_client_includes *.h)
install(FILES ${ft_client_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot)

add_executable(fetchdatatest fetchdatatest.cpp)
target_link_libraries(fetchdatatest foxtrotclient)

add_executable(flagtest flagtest.cpp)
target_link_libraries(flagtest foxtrotclient)
