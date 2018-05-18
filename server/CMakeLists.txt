
set(FOXTROT_PROTO_VERSION 2)
#set(FOXTROT_PROTO_HASH "cbdcdb06ae7c6d8b95bc08a26ab732ee521b0854")
set(FOXTROT_PROTO_HASH "6d35a7fd69a577a452f5ec4aacf4ee20b11a44a0")

file(SHA1 ${CMAKE_CURRENT_SOURCE_DIR}/foxtrot.proto THIS_PROTO_HASH)
message(STATUS "ft_proto hash: ${THIS_PROTO_HASH}")

if(NOT ${FOXTROT_PROTO_HASH} STREQUAL ${THIS_PROTO_HASH})
    message(ERROR "mismatch of foxtrot proto file hash! Need version increment!")
endif()


protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS foxtrot.proto)
PROTOBUF_GENERATE_GRPC_CPP(PROTO_GRPC_SRCS PROTO_GRPC_HDRS foxtrot.proto)

install(FILES foxtrot.proto DESTINATION ${CMAKE_INSTALL_DATADIR}/foxtrot/)

message("generated protobuf stubs: ${PROTO_SRCS}")
message("generated protobuf hdrs: ${PROTO_HDRS}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(foxtrotserv ServerImpl.cpp ServerDescribeImpl.cpp  InvokeCapabilityImpl.cpp  DeviceHarness.cpp 
FetchDataImpl.cpp ExperimentalSetup.cpp ServerUtil.cpp ChunkStreamImpl.cpp ServerFlagsImpl.cpp
ServerCommandImpl.cpp config.cpp
${PROTO_SRCS} ${PROTO_GRPC_SRCS} ${PROTO_HDRS} ${PROTO_GRPC_HDRS}
)



file(GLOB ft_server_includes *.h)
install(FILES ${ft_server_includes} ${PROTO_HDRS} ${PROTO_GRPC_HDRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/)

target_link_libraries(foxtrotserv PUBLIC foxtrot ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES} ${OPENSSL_LIBRARIES} 
${ZLIB_LIBRARIES} ${dl})

target_include_directories(foxtrotserv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS foxtrotserv LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/ ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

add_executable(exptserve exptserve.cpp exptserve_funcs.cpp)

#NOTE: WTF? If you don't link foxtrot here, it seems to have some weird gprof error 
target_link_libraries(exptserve foxtrotserv ${Boost_PROGRAM_OPTIONS_LIBRARY}
${Boost_FILESYSTEM_LIBRARY})

if(FOXTROT_CONAN_BUILD)
    target_include_directories(exptserve PRIVATE ${CONAN_INCLUDE_DIRS_BOOST_PROGRAM_OPTIONS})
endif() 

install(TARGETS exptserve RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(testClient testClient.cpp)
target_link_libraries(testClient foxtrotserv foxtrotclient)
target_include_directories(testClient PRIVATE ${CMAKE_SOURCE_DIR}/client/)


add_executable(harnesstest harnesstest.cpp)
target_include_directories(harnesstest PRIVATE ${CMAKE_SOURCE_DIR}/devices/dummy/)
target_link_libraries(harnesstest ${OPENSSL_LIBRARIES} foxtrot dummy foxtrotserv)
