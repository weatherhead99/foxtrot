file(GLOB proto_files CONFIGURE_DEPENDS *.proto)

if(TARGET gRPC::grpc_cpp_plugin)
  message(STATUS "using gRPC target to find plugin program")
  get_target_property(GRPC_CPP_PLUGIN_PROGRAM  gRPC::grpc_cpp_plugin  LOCATION)
else()
  message(STATUS "gRPC target not declared using old-style variable name")
endif()

message("grpc_plugin_program: ${GRPC_CPP_PLUGIN_PROGRAM}")

if(NOT GRPC_CPP_PLUGIN_PROGRAM)
    message(FATAL_ERROR "couldn't find grpc plugin program")
endif()
    
protobuf_generate(OUT_VAR PROTO_HDRS  PROTOS ${proto_files} PLUGIN protoc-gen-grpc=${GRPC_CPP_PLUGIN_PROGRAM} PROTOC_OPTIONS --grpc_out=${CMAKE_CURRENT_BINARY_DIR})

#asio_grpc_protobuf_generate(PROTOS ${proto_files} OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
#  OUT_VAR ASIO_PROTO_HDRS TARGET GENERATE_GRPC) 


set(GEN_SRCS "")
foreach(src ${proto_files})
  get_filename_component(bn ${src} NAME_WE)
  list(APPEND GEN_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${bn}.pb.cc)
  list(APPEND GEN_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${bn}.pb.h)
  list(APPEND GEN_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${bn}.grpc.pb.cc)
  list(APPEND GEN_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${bn}.grpc.pb.h)
endforeach()
set_property(SOURCE ${GEN_SRCS} PROPERTY GENERATED TRUE)


#message("gen_srcs: ${GEN_SRCS}")

add_library(foxtrot_protos OBJECT ${PROTO_HDRS} ${GEN_SRCS})
#add_library(foxtrot_protos OBJECT ${ASIO_PROTO_HDRS})
target_link_libraries(foxtrot_protos PUBLIC protobuf::libprotobuf gRPC::grpc++ gRPC::grpc)

add_custom_target(copy_protos
  ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/*.h ${CMAKE_BINARY_DIR}/foxtrot/
  DEPENDS foxtrot_protos)

install(FILES ${proto_files} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/foxtrot/protos/)
