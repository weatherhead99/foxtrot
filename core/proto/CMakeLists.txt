set(proto_files foxtrot.proto
  ft_types.proto
  ft_auth.proto
  ft_error.proto
  ft_streams.proto
  ft_capability.proto
  ft_sessions.proto
  ft_flags.proto)


message("grpc_plugin_program: ${GRPC_CPP_PLUGIN_PROGRAM}")

protobuf_generate(OUT_VAR PROTO_HDRS  PROTOS ${proto_files} PLUGIN protoc-gen-grpc=${GRPC_CPP_PLUGIN_PROGRAM} PROTOC_OPTIONS --grpc_out=${CMAKE_CURRENT_BINARY_DIR})

set(GEN_SRCS "")
foreach(src ${proto_files})
  get_filename_component(bn ${src} NAME_WE)
  list(APPEND GEN_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${bn}.grpc.pb.cc)
endforeach()
set_property(SOURCE ${GEN_SRCS} PROPERTY GENERATED TRUE)


message("gen_srcs: ${GEN_SRCS}")

add_library(foxtrot_protos OBJECT ${PROTO_HDRS} ${GEN_SRCS})
message(STATUS "pb_include_dirs: ${PROTOBUF_INCLUDE_DIRS}")
target_include_directories(foxtrot_protos PUBLIC ${PROTOBUF_INCLUDE_DIRS})

add_custom_target(copy_protos
  ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/*.h ${CMAKE_BINARY_DIR}/foxtrot/
  DEPENDS foxtrot_protos)
