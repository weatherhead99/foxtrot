add_library(dummydevice STATIC dummy/dummyDevice.cpp)
target_link_libraries(dummydevice foxtrot_server)

#have this here as a one off as it's usually built with server package
target_include_directories(dummydevice PRIVATE ${CMAKE_BINARY_DIR})

install(TARGETS dummydevice EXPORT foxtrot_server
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)
