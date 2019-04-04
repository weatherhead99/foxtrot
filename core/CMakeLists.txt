project(foxtrot_core C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
#in case of building static libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message(WARNING "conanbuildinfo detected, building using
    conan dependencies")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(FOXTROT_CORE_CONAN_BUILD TRUE)
    conan_basic_setup()
endif()


find_package(Boost REQUIRED COMPONENTS log)
find_package(Protobuf REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
find_package(grpc REQUIRED)
if(${GRPC_CPP_PLUGIN} STREQUAL GRPC_CPP_PLUGIN-NOTFOUND)
    message(FATAL_ERROR "can't compile GRPC code, can't continue!")
endif()

#need to generate export headers for windows DLL compat
include(GenerateExportHeader)
include(GNUInstallDirs)

#build foxtrot core library (errors and logging)
set(srcs src/backward.cpp src/DeviceError.cpp src/Logging.cpp
src/ProtocolError.cpp src/ProtocolTimeoutError.cpp src/StubError.cpp src/ContentionError.cpp src/Error.cpp src/ProtocolError.cpp src/ServerError.cpp src/TelemetryError.cpp)

#generate protobuf headers
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS proto/foxtrot.proto)
protobuf_generate_grpc_cpp(PROTO_GRPC_SRCS PROTO_GRPC_HDRS proto/foxtrot.proto)

add_library(foxtrot_core ${srcs} ${PROTO_SRCS} ${PROTO_GRPC_SRCS})
generate_export_header(foxtrot_core
EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/foxtrot_core_export.h)

target_include_directories(foxtrot_core PUBLIC include)
target_include_directories(foxtrot_core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})


install(TARGETS foxtrot_core 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    
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
        
