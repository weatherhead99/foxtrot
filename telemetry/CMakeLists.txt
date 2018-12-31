
set(telem_srcs TelemetryServer.cpp TelemetryClient.cpp TelemetryTransport.cpp)

if(BUILD_NANOMSG_TRANSPORT)
    list(APPEND telem_srcs NanomsgTransport.cpp)
    if(FOXTROT_CONAN_BUILD)
        set(NANOMSG_LIBRARIES ${CONAN_LIBS_NANOMSG})
  
    else()
        include(FindPkgConfig)
        pkg_search_module(NANOMSG nanomsg libnanomsg)
        if(NOT NANOMSG_LIBRARIES)
            message(FATAL_ERROR "couldn't find nanomsg libraries")
        endif()
    endif()
    
    message(STATUS "nanomsg libraries: ${NANOMSG_LIBRARIES}")
endif()

if(BUILD_MQTT_TRANSPORT)
    list(APPEND telem_srcs MQTTTransport.cpp)
    
    find_library(mqttpp_lib mosquittopp)
    if(NOT mqttpp_lib)
        message(FATAL_ERROR "couldn't find mosquittopp library")
    endif()
    message(STATUS "mosquittopp libraries: ${mqttpp_lib}")
    
endif()


add_library(foxtrot_telemetry ${telem_srcs})
target_link_libraries(foxtrot_telemetry foxtrotclient foxtrotserv ${NANOMSG_LIBRARIES} ${anl} ${mqttpp_lib})

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
