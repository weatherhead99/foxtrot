project(foxtrot_core C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
#in case of building static libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#import other cmake functionality we need later
include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

#setup conan build if necessary
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message(WARNING "conanbuildinfo detected, building using
    conan dependencies")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(FOXTROT_CORE_CONAN_BUILD TRUE)
    conan_basic_setup()
endif()

#set shared library build by default
set(BUILD_SHARED_LIBS ON CACHE BOOL "build shared libraries")

find_package(Boost REQUIRED COMPONENTS log)
find_package(Protobuf REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
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

add_library(foxtrot_core ${srcs} ${PROTO_SRCS} ${PROTO_GRPC_SRCS})

generate_export_header(foxtrot_core
EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/foxtrot_core_export.h)

target_include_directories(foxtrot_core PUBLIC 
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:/${CMAKE_INSTALL_INCLUDEDIR}>)
target_include_directories(foxtrot_core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(foxtrot_core PUBLIC protobuf::libprotobuf Boost::log ${GRPC_LIBRARIES})


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
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/
        COMPONENT DEVEL
        NAMESPACE foxtrot)

#generate cmake package file
configure_package_config_file(cmake/foxtrotCoreConfig.cmake.in
                              foxtrotCoreConfig.cmake
                              INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/)
#and install it 
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrotCoreConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/
        COMPONENT DEVEL)
