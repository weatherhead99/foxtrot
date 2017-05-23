include(GNUInstallDirs)

install(TARGETS foxtrot EXPORT ft_targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/ )
file(GLOB ft_includes ${CMAKE_SOURCE_DIR}/include/*.h)
message(STATUS "ft includes: ${ft_includes}")
install(FILES ${ft_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ COMPONENT devel)


#cmake package files
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/foxtrotConfig.cmake.in foxtrotConfig.cmake 
			      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrotConfig.cmake DESTINATION 
	      ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/ COMPONENT devel)


write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/foxtrotVersion.cmake VERSION 0.0.1 
			      COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrotVersion.cmake 
	 DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/ COMPONENT devel)


