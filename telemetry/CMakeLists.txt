find_library(nanomsg nanomsg)


if(NOT nanomsg)
    message(ERROR "couldn't find nanomsg library...")
endif()

message("nanomsg: ${nanomsg}")


add_library(foxtrot_telemetry TelemetryServer.cpp)
target_link_libraries(foxtrot_telemetry foxtrotclient)
