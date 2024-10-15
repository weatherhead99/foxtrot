file(GLOB proto_files CONFIGURE_DEPENDS *.proto)

protobuf_generate(OUT_VAR PROTO_GEN LANGUAGE cpp PROTOS ${proto_files})
protobuf_generate(OUT_VAR GRPC_GEN LANGUAGE grpc PROTOS ${proto_files} PLUGIN protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin> GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc)


message(STATUS "generated proto files: ${PROTO_GEN}")
message(STATUS "generated gRPC files: ${GRPC_GEN}")

#asio_grpc_protobuf_generate(PROTOS ${proto_files} OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
#  OUT_VAR ASIO_PROTO_HDRS TARGET GENERATE_GRPC) 

add_library(foxtrot_protos OBJECT ${GRPC_GEN} ${PROTO_GEN})
#add_library(foxtrot_protos OBJECT ${ASIO_PROTO_HDRS})
target_link_libraries(foxtrot_protos PUBLIC protobuf::libprotobuf gRPC::grpc++ gRPC::grpc)

add_custom_target(copy_protos
  ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/*.h ${CMAKE_BINARY_DIR}/foxtrot/
  DEPENDS foxtrot_protos)

install(FILES ${proto_files} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/foxtrot/protos/)
