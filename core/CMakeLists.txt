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
include(GitVersion)
include(FoxtrotCommonSetup)

#setup conan build if necessary
message(STATUS "checking for conan build...")
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message(WARNING "conanbuildinfo detected, building using
    conan dependencies")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(FOXTROT_CORE_CONAN_BUILD TRUE)
    conan_basic_setup()
endif()


set(packname "core")

configure_file(${CMAKE_SOURCE_DIR}/share/version.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
configure_file(${CMAKE_SOURCE_DIR}/share/version.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/${packname}_version.h)

message("foxtrot core version: ${VERSION}")

find_package(Boost REQUIRED COMPONENTS log)
find_package(Protobuf REQUIRED)

find_package(grpc REQUIRED)
if(${GRPC_CPP_PLUGIN} STREQUAL GRPC_CPP_PLUGIN-NOTFOUND)
    message(FATAL_ERROR "can't compile GRPC code, can't continue!")
endif()


#build foxtrot core library (errors and logging)
set(srcs src/backward.cpp src/DeviceError.cpp src/Logging.cpp
src/ProtocolError.cpp src/ProtocolTimeoutError.cpp src/StubError.cpp src/ContentionError.cpp src/Error.cpp src/ProtocolError.cpp src/ServerError.cpp src/TelemetryError.cpp src/config.cpp)

#generate protobuf headers
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS proto/foxtrot.proto)
protobuf_generate_grpc_cpp(PROTO_GRPC_SRCS PROTO_GRPC_HDRS proto/foxtrot.proto)


add_library(foxtrot_core ${srcs} ${PROTO_SRCS} ${PROTO_GRPC_SRCS}
            ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
foxtrot_generate_export_header(${packname} foxtrot_core)

add_custom_command(TARGET foxtrot_core POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${PROTO_HDRS} ${PROTO_GRPC_HDRS} 
${CMAKE_CURRENT_BINARY_DIR}/foxtrot/)
foxtrot_standard_include_dirs(foxtrot_core)

target_link_libraries(foxtrot_core PUBLIC protobuf::libprotobuf Boost::log ${GRPC_LIBRARIES})


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
