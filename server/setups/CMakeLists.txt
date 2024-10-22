add_library(dummy_setup MODULE dummy_setup.cpp)
target_link_libraries(dummy_setup PUBLIC foxtrot_server)
target_link_libraries(dummy_setup PRIVATE dummydevice)
target_include_directories(dummy_setup PRIVATE ${CMAKE_BINARY_DIR})

set_target_properties(dummy_setup PROPERTIES PREFIX "")

install(TARGETS dummy_setup EXPORT foxtrot_server
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/ft_setups/)
