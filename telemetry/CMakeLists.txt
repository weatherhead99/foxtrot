find_library(nanomsg nanomsg)

if(NOT nanomsg)
    message(ERROR "couldn't find nanomsg library...")
endif()

message("nanomsg: ${nanomsg}")


add_library(foxtrot_telemetry TelemetryServer.cpp)
target_link_libraries(foxtrot_telemetry foxtrotclient foxtrotserv ${nanomsg})

target_include_directories(foxtrot_telemetry INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(telem_test telem_test.cpp)
target_link_libraries(telem_test foxtrot_telemetry)

install(TARGETS foxtrot_telemetry LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot 
ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)


add_executable(telem_bcast telem_bcast.cpp telem_util.cpp)
target_link_libraries(telem_bcast foxtrot_telemetry ${Boost_PROGRAM_OPTIONS_LIBRARY})

install(TARGETS telem_bcast RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})