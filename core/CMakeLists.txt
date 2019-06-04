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

find_package(Boost REQUIRED COMPONENTS log program_options filesystem)
find_package(Protobuf REQUIRED)

find_package(gRPC REQUIRED)
if(FOXTROT_CORE_CONAN_BUILD)
    find_package(protoc REQUIRED)
endif()

#build foxtrot core library (errors and logging)

set(srcs src/backward.cpp src/DeviceError.cpp src/Logging.cpp
src/ProtocolError.cpp src/ProtocolTimeoutError.cpp src/StubError.cpp src/ContentionError.cpp src/Error.cpp src/ProtocolError.cpp src/ServerError.cpp src/TelemetryError.cpp src/config.cpp)

get_filename_component(protopath proto/foxtrot.proto ABSOLUTE)
find_program(grpc_plugin grpc_cpp_plugin)


set(proto_srcs "foxtrot.pb.cc;foxtrot.pb.h")

add_custom_command(OUTPUT ${proto_srcs}
                    COMMAND protobuf::protoc
                    ARGS --cpp_out ${CMAKE_CURRENT_BINARY_DIR} foxtrot.proto -I${CMAKE_CURRENT_SOURCE_DIR}/proto
                    COMMENT "generating protobuf code from ${protopath}"
                    DEPENDS ${protopath} protobuf::protoc
                    VERBATIM)
set_source_files_properties(${proto_srcs} PROPERTIES GENERATED TRUE)

set(grpc_srcs "foxtrot.grpc.pb.cc;foxtrot.grpc.pb.h")

add_custom_command(OUTPUT ${grpc_srcs}
                    COMMAND protobuf::protoc
                    ARGS --grpc_out ${CMAKE_CURRENT_BINARY_DIR} --plugin=protoc-gen-grpc=${grpc_plugin} foxtrot.proto -I${CMAKE_CURRENT_SOURCE_DIR}/proto
                    DEPENDS ${protopath} protobuf::protoc
                    COMMENT "generating GRPC stub code from ${protopath}"
                    VERBATIM)
set_source_files_properties(${grpc_srcs} PROPERTIES GENERATED TRUE)

message("generated sources: ${grpc_srcs}")

add_library(foxtrot_core ${srcs} ${proto_srcs} ${grpc_srcs} ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
foxtrot_standard_include_dirs(foxtrot_core)
foxtrot_generate_export_header(${packname} foxtrot_core)

set(gen_hdrs "foxtrot.pb.h;foxtrot.grpc.pb.h")

add_custom_command(TARGET foxtrot_core POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${gen_hdrs}
 ${CMAKE_CURRENT_BINARY_DIR}/foxtrot)

target_link_libraries(foxtrot_core PUBLIC Boost::log Boost::program_options Boost::filesystem)


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

install(FILES proto/foxtrot.proto
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
        ${CMAKE_CURRENT_LIST_DIR}/cmake/Findgrpc.cmake
        DESTINATION ${ft_cmakedir})

        
#add to package registry
foxtrot_add_to_package_registry(foxtrot_core foxtrotCore)
