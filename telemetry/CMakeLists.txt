
if(FOXTROT_CONAN_BUILD)
  set(NANOMSG_LIBRARIES ${CONAN_LIBS_NANOMSG})
  
else()
  include(FindPkgConfig)
  pkg_search_module(NANOMSG nanomsg libnanomsg)
endif()
  
message(STATUS "nanomsg libraries: ${NANOMSG_LIBRARIES}")


# if(FOXTROT_CONAN_BUILD)
#   find_library(anl anl)
#   message(STATUS "libanl: ${anl}")
  
# endif()


add_library(foxtrot_telemetry TelemetryServer.cpp TelemetryClient.cpp TelemetryTransport.cpp NanomsgTransport.cpp)
target_link_libraries(foxtrot_telemetry foxtrotclient foxtrotserv ${NANOMSG_LIBRARIES} ${anl})

target_include_directories(foxtrot_telemetry INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${NANOMSG_INCLUDE_DIRS})



add_executable(telem_test telem_test.cpp)
target_link_libraries(telem_test foxtrot_telemetry )

install(TARGETS foxtrot_telemetry LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

file(GLOB telem_includes *.h)
install(FILES ${telem_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/)

add_executable(telem_bcast telem_bcast.cpp telem_util.cpp)
target_link_libraries(telem_bcast foxtrot_telemetry ${Boost_PROGRAM_OPTIONS_LIBRARY} )
install(TARGETS telem_bcast RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


add_executable(telem_recv_test telem_recv_test.cpp)
target_link_libraries(telem_recv_test foxtrot_telemetry ${Boost_DATE_TIME_LIBRARY})
