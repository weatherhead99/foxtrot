

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS foxtrot.proto)
PROTOBUF_GENERATE_GRPC_CPP(PROTO_GRPC_SRCS PROTO_GRPC_HDRS foxtrot.proto)

message("generated protobuf stubs: ${PROTO_SRCS}")
message("generated protobuf hdrs: ${PROTO_HDRS}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(foxtrotserv SHARED ServerImpl.cpp ServerDescribeImpl.cpp  InvokeCapabilityImpl.cpp  DeviceHarness.cpp 
ExperimentalSetup.cpp
${PROTO_SRCS} ${PROTO_GRPC_SRCS} ${PROTO_HDRS} ${PROTO_GRPC_HDRS}
)

target_link_libraries(foxtrotserv foxtrot ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES} ${OPENSSL_LIBRARIES} 
${ZLIB_LIBRARIES} ${dl} dummy)

add_executable(exptserve exptserve.cpp)

#NOTE: WTF? If you don't link foxtrot here, it seems to have some weird gprof error 
target_link_libraries(exptserve foxtrotserv dummy ${Boost_PROGRAM_OPTIONS_LIBRARY})

add_executable(testClient testClient.cpp)
target_link_libraries(testClient foxtrotserv foxtrotclient)
target_include_directories(testClient PRIVATE ${CMAKE_SOURCE_DIR}/client/)


add_executable(harnesstest harnesstest.cpp)
target_include_directories(harnesstest PRIVATE ${CMAKE_SOURCE_DIR}/devices/dummy/)
target_link_libraries(harnesstest foxtrot dummy foxtrotserv)
