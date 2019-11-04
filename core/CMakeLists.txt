project(foxtrot_core C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
#in case of building static libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/)

#import other cmake functionality we need later
include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FoxtrotCommonSetup)

#setup conan build if necessary
message(STATUS "checking for conan build...")
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message(WARNING "conanbuildinfo detected, building using
    conan dependencies")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(FOXTROT_CORE_CONAN_BUILD TRUE)
    conan_basic_setup()
    set(HASH "None")
    set(REFSPEC "None")
    set(TAG "None")
    set(TAGGED "false")
    foxtrot_writeout_build_directory(.builddir.info)
  else()
    message(STATUS "not a conan build")
    include(GitVersion)    
endif()


set(packname "core")

configure_file(${CMAKE_SOURCE_DIR}/share/version.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
configure_file(${CMAKE_SOURCE_DIR}/share/version.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/${packname}_version.h)

message("foxtrot core version: ${VERSION}")

find_package(Boost REQUIRED COMPONENTS log program_options filesystem date_time)
find_package(Protobuf REQUIRED)
find_package(rttr 0.9.6 REQUIRED)

find_package(gRPC REQUIRED)
if(FOXTROT_CORE_CONAN_BUILD)
    find_package(protoc REQUIRED)
endif()

#message(WARNING "using protoc program: ${protobuf::protoc}")

#build foxtrot core library (errors and logging)

set(srcs src/backward.cpp src/DeviceError.cpp src/Logging.cpp
src/ProtocolError.cpp src/ProtocolTimeoutError.cpp src/StubError.cpp src/ContentionError.cpp src/Error.cpp src/ProtocolError.cpp src/ServerError.cpp src/TelemetryError.cpp src/ReflectionError.cpp src/config.cpp src/typeUtil.cpp src/ft_tuple_helper.cc)


find_program(grpc_plugin grpc_cpp_plugin)


function(generate_proto_and_grpc)
    include(CMakeParseArguments)
    set(multiValueArgs PROTO_FILES)
    set(singleValueArgs OUT_ALL_VARNAME OUT_HDR_VARNAME)
    cmake_parse_arguments(generate_proto_and_grpc "" "${singleValueArgs}"  "${multiValueArgs}" ${ARGN})
    set(all_srcs "")
    set(all_genhdrs "")
    foreach(protofile ${generate_proto_and_grpc_PROTO_FILES})
        get_filename_component(basename ${protofile} NAME_WE)
        get_filename_component(protopath ${protofile} ABSOLUTE)
        set(proto_srcs "${basename}.pb.cc;${basename}.pb.h")
        set(grpc_srcs "${basename}.grpc.pb.cc;${basename}.grpc.pb.h")
        set(gen_hdrs "${basename}.grpc.pb.h;${basename}.pb.h")

        list(APPEND all_srcs ${proto_srcs})
        list(APPEND all_srcs ${grpc_srcs})
        list(APPEND all_genhdrs ${gen_hdrs})
        
        add_custom_command(OUTPUT ${proto_srcs}
                           COMMAND protobuf::protoc
                           ARGS --cpp_out ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${protofile} -I${CMAKE_CURRENT_SOURCE_DIR}/proto
                           COMMENT "generating protobuf code from ${protopath}"
                           DEPENDS ${protopath} protobuf::protoc
                           VERBATIM)
        set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/${proto_srcs} PROPERTIES GENERATED TRUE)

        add_custom_command(OUTPUT ${grpc_srcs}
                           COMMAND protobuf::protoc
                           ARGS --grpc_out ${CMAKE_CURRENT_BINARY_DIR} --plugin=protoc-gen-grpc=${grpc_plugin} ${CMAKE_CURRENT_SOURCE_DIR}/${protofile} -I${CMAKE_CURRENT_SOURCE_DIR}/proto
                           DEPENDS ${protopath} protobuf::protoc
                           COMMENT "generating GRPC stub code from ${protopath}"
                           VERBATIM)
        set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/${grpc_srcs} PROPERTIES GENERATED TRUE)
        
        add_custom_target(${basename}_copy_includes
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/foxtrot
        COMMAND ${CMAKE_COMMAND} -E copy ${gen_hdrs} ${CMAKE_CURRENT_BINARY_DIR}/foxtrot
	DEPENDS ${grpc_srcs} ${proto_srcs}
        COMMENT "copying generated headers to top level build tree")
      
        
    endforeach()
    set(${generate_proto_and_grpc_OUT_ALL_VARNAME} ${all_srcs} PARENT_SCOPE)
    set(${generate_proto_and_grpc_OUT_HDR_VARNAME} ${all_genhdrs} PARENT_SCOPE)
    
endfunction()

set(proto_input_files proto/foxtrot.proto proto/ft_types.proto proto/ft_auth.proto
proto/ft_error.proto proto/ft_streams.proto proto/ft_capability.proto)
generate_proto_and_grpc(PROTO_FILES ${proto_input_files} OUT_ALL_VARNAME pb_srcs OUT_HDR_VARNAME pb_hdrs)
message("pb_srcs: ${pb_srcs}")
message("pb_hdrs: ${pb_hdrs}")



add_library(foxtrot_core ${srcs} ${pb_srcs} ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
foxtrot_standard_include_dirs(foxtrot_core)
foxtrot_generate_export_header(${packname} foxtrot_core)

add_dependencies(foxtrot_core ft_types_copy_includes)
add_dependencies(foxtrot_core foxtrot_copy_includes)
add_dependencies(foxtrot_core ft_error_copy_includes)
add_dependencies(foxtrot_core ft_auth_copy_includes)

target_include_directories(foxtrot_core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})


target_link_libraries(foxtrot_core PUBLIC Boost::log Boost::program_options Boost::filesystem Boost::date_time protobuf::libprotobuf gRPC::grpc++ RTTR::Core)


set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotCore/)
set(ft_core_sourcedir ${CMAKE_CURRENT_SOURCE_DIR})
set(ft_core_binarydir ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS foxtrot_core 
    EXPORT foxtrot_core
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)
    
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/
        COMPONENT devel)

install(FILES ${proto_input_files}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/foxtrot/
        COMPONENT devels)

install(EXPORT foxtrot_core
        DESTINATION ${ft_cmakedir}
        COMPONENT devel
        NAMESPACE foxtrot::)

foxtrot_create_package_config(cmake/foxtrotCoreConfig.cmake.in ${ft_cmakedir} 
    "ft_cmakedir;ft_core_sourcedir;ft_core_binarydir")

#install cmake macro files
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/FoxtrotCommonSetup.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake.in
        ${CMAKE_CURRENT_LIST_DIR}/cmake/Findlibusb.cmake
        DESTINATION ${ft_cmakedir})

        
#add to package registry
foxtrot_add_to_package_registry(foxtrot_core foxtrotCore)

add_subdirectory(devprogs)
