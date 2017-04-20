find_library(nanomsg nanomsg)


if(NOT nanomsg)
    message(ERROR "couldn't find nanomsg library...")
endif()

message("nanomsg: ${nanomsg}")


add_library(foxtrot_telemetry TelemetryServer.cpp)
target_link_libraries(foxtrot_telemetry foxtrotclient foxtrotserv ${nanomsg})

add_executable(telem_test telem_test.cpp)
target_link_libraries(telem_test foxtrot_telemetry)

install(TARGETS foxtrot_telemetry LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtot ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

file(GLOB telem_includes *.h)
install(FILES ${telem_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/)
