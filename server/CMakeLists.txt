project(foxtrot_server C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(packname "server")

find_package(foxtrotCore REQUIRED)

include(GenerateExportHeader)


message("module dir: ${CMAKE_MODULE_PATH}")
include(FoxtrotCommonSetup)


set(server_srcs src/Device.cpp)

set(exptserve_srcs src/ServerImpl.cpp src/ServerDescribeImpl.cpp 
    src/InvokeCapabilityImpl.cpp src/DeviceHarness.cpp src/FetchDataImpl.cpp 
    src/ExperimentalSetup.cpp src/ServerUtil.cpp src/ChunkStreamImpl.cpp 
    src/ServerFlagsImpl.cpp src/ServerCommandImpl.cpp src/BroadcastNotificationImpl.cpp
    src/config.cpp src/FetchDataImpl.cpp src/exptserve.cpp src/exptserve_funcs.cpp
    src/pushbullet_api.cpp
    )

add_library(foxtrot_server ${server_srcs})
foxtrot_generate_export_header(${packname} foxtrot_server)
target_include_directories(foxtrot_server PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    
target_include_directories(foxtrot_server PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

add_executable(exptserve ${exptserve_srcs})
target_include_directories(exptserve PRIVATE include/exptserve/)
target_include_directories(exptserve PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(exptserve PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    
target_link_libraries(exptserve foxtrot::foxtrot_core)
