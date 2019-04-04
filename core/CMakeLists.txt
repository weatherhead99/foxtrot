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
src/ProtocolError.cpp src/ProtocolTimeoutError.cpp src/StubError.cpp src/ContentionError.cpp src/Error.cpp src/ProtocolError.cpp src/ServerError.cpp src/TelemetryError.cpp)

#generate protobuf headers
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS proto/foxtrot.proto)
protobuf_generate_grpc_cpp(PROTO_GRPC_SRCS PROTO_GRPC_HDRS proto/foxtrot.proto)

add_library(foxtrot_core ${srcs} ${PROTO_SRCS} ${PROTO_GRPC_SRCS}
            ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

generate_export_header(foxtrot_core
EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/foxtrot_core_export.h)

target_include_directories(foxtrot_core PUBLIC 
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:/${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(foxtrot_core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(foxtrot_core PUBLIC protobuf::libprotobuf Boost::log ${GRPC_LIBRARIES})


set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/)

install(TARGETS foxtrot_core 
    EXPORT foxtrot_core
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)
    
install(DIRECTORY include/foxtrot
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT devel)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/foxtrot_core_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/
        COMPONENT devel)

install(FILES ${PROTO_HDRS} ${PROTO_GRPC_HDRS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/
        COMPONENT devel)
        
install(FILES proto/foxtrot.proto
        DESTINATION ${CMAKE_INSTALL_DATADIR}/foxtrot/
        COMPONENT DEVEL)
        
install(EXPORT foxtrot_core
        DESTINATION ${ft_cmakedir}
        COMPONENT DEVEL
        NAMESPACE foxtrot)

#generate cmake package file
configure_package_config_file(cmake/foxtrotCoreConfig.cmake.in
                              foxtrotCoreConfig.cmake
                              INSTALL_DESTINATION ${ft_cmakedir}
                              PATH_VARS ft_cmakedir)
#and install it 
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrotCoreConfig.cmake
        DESTINATION ${ft_cmakedir}
        COMPONENT DEVEL)

#install cmake macro files
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/FoxtrotCommonSetup.cmake
        DESTINATION ${ft_cmakedir})
