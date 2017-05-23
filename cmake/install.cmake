
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME runtime)


#foxtrot
install(TARGETS foxtrot EXPORT foxtrot LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/ 
	  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)
	  
file(GLOB ft_includes ${CMAKE_SOURCE_DIR}/include/*.h)
message(STATUS "ft includes: ${ft_includes}")
install(FILES ${ft_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ COMPONENT devel)

#server


file(GLOB ft_server_includes ${CMAKE_SOURCE_DIR}/server/*.h)
install(FILES ${ft_server_includes} ${PROTO_HDRS} ${PROTO_GRPC_HDRS} 
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ COMPONENT devel)



#client

file(GLOB ft_client_includes ${CMAKE_SOURCE_DIR}/client/*.h)
install(FILES ${ft_client_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot COMPONENT devel)

	

#telemetry
	
file(GLOB telem_includes ${CMAKE_SOURCE_DIR}/telemetry/*.h)
install(FILES ${telem_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ COMPONENT devel)


	
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


