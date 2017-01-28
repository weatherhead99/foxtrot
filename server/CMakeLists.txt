find_package(Protobuf  REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS foxtrot.proto)

message("generated protobuf stubs: ${PROTO_SRCS}")
message("generated protobuf hdrs: ${PROTO_HDRS}")

add_executable(exptserve exptserve.cpp ${PROTO_SRCS})
target_link_libraries(exptserve ${PROTOBUF_LIBRARIES})
