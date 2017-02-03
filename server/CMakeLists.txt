

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS foxtrot.proto)
PROTOBUF_GENERATE_GRPC_CPP(PROTO_GRPC_SRCS PROTO_GRPC_HDRS foxtrot.proto)

message("generated protobuf stubs: ${PROTO_SRCS}")
message("generated protobuf hdrs: ${PROTO_HDRS}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(foxtrotserv SHARED ServerImpl.cpp ${PROTO_SRCS} ${PROTO_GRPC_SRCS} ${PROTO_HDRS} ${PROTO_GRPC_HDRS})

add_executable(exptserve exptserve.cpp)

if(CONAN_BUILD)
target_link_libraries(exptserve foxtrotserv ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES} ${dl})
else()
target_link_libraries(exptserve foxtrotserv ${PROTOBUF_LIBRARIES} gRPC::grpc++)
endif()





