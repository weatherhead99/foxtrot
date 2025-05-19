#find the protobuf source files
file(GLOB proto_files CONFIGURE_DEPENDS *.proto)

#generate the sources, first for .pb.h and .pb.cc
#then for .grpc.pb.h  and .grpc.pb.cc
message(DEBUG "proto files: ${proto_files}")

get_target_property(PROTOC_EXE protobuf::protoc LOCATION)
message(STATUS "protoc compiler: ${PROTOC_EXE}")

get_target_property(PLUGIN_EXE gRPC::grpc_cpp_plugin LOCATION)
message("grpc plugin: ${PLUGIN_EXE}")


protobuf_generate(OUT_VAR PROTO_GEN LANGUAGE cpp PROTOS ${proto_files})
protobuf_generate(OUT_VAR GRPC_GEN LANGUAGE grpc PROTOS ${proto_files} PLUGIN protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin> GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PROTOC_OUT_DIR)

#annoyingly, protobuf_generate returns it all in one variable,
#split it into headers and sources (so we can use "modern cmake" capability to trakc
#headers etc
set(ALL_GEN "${PROTO_GEN};${GRPC_GEN}")
set(GEN_HDRS ${ALL_GEN})
set(GEN_SRCS ${ALL_GEN})

list(FILTER GEN_HDRS INCLUDE REGEX .h$)
list(FILTER GEN_SRCS INCLUDE REGEX .cc$)


#asio_grpc_protobuf_generate(PROTOS ${proto_files} OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
#  OUT_VAR ASIO_PROTO_HDRS TARGET GENERATE_GRPC) 

#ok, actually build the library
add_library(foxtrot_protos OBJECT)
target_sources(foxtrot_protos PUBLIC FILE_SET HEADERS FILES ${GEN_HDRS} BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR})
target_sources(foxtrot_protos PRIVATE ${GEN_SRCS})


#add_library(foxtrot_protos OBJECT ${ASIO_PROTO_HDRS})
target_link_libraries(foxtrot_protos PUBLIC protobuf::libprotobuf gRPC::grpc++)
target_include_directories(foxtrot_protos PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
$<INSTALL_INTERFACE:include/foxtrot>)

#install both the protobuf definitions and the target itself
install(FILES ${proto_files} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/foxtrot/proto/)

install(TARGETS foxtrot_protos
  EXPORT foxtrot_core
  PUBLIC_HEADER
  COMPONENT devel
  FILE_SET HEADERS
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/proto
)
