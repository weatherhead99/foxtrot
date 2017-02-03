find_package(Protobuf  REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)
find_package(grpc REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
include_directories(${GRPCPP_INCLUDE_DIR})

#find protobuf plugin

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS foxtrot.proto)
PROTOBUF_GENERATE_GRPC_CPP(PROTO_GRPC_SRCS PROTO_GRPC_HDRS foxtrot.proto)

message("generated protobuf stubs: ${PROTO_SRCS}")
message("generated protobuf hdrs: ${PROTO_HDRS}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(foxtrotserv ServerImpl.cpp)
target_link_libraries(foxtrotserv ${GRPC_LIBRARIES} ${ZLIB_LIBRARY} ${OPENSSL_LIBRARIES})

add_executable(exptserve exptserve.cpp ${PROTO_SRCS} ${PROTO_GRPC_HDRS})
target_link_libraries(exptserve foxtrotserv ${PROTOBUF_LIBRARIES} ${OPENSSL_LIBRARIES} )
