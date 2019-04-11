project(foxtrot C CXX)
cmake_minimum_required(VERSION 3.6)

include(ExternalProject)

option(BUILD_SERVER "build foxtrot server component" ON)
option(BUILD_CLIENT "build foxtrot client component" ON)
option(BUILD_PROTOCOLS "build extra foxtrot communication protocols" ON)
option(BUILD_DEVICES "build device drivers" ON)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/dist 
    CACHE PATH "installation prefix" FORCE)

message(STATUS "building foxtrot core library")
ExternalProject_Add(foxtrot_core SOURCE_DIR ${CMAKE_SOURCE_DIR}/core
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

if(BUILD_SERVER)
    message(STATUS "building foxtrot server components")
    ExternalProject_Add(foxtrot_server SOURCE_DIR ${CMAKE_SOURCE_DIR}/server
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
                        DEPENDS foxtrot_core)
endif()

if(BUILD_CLIENT)
    message(STATUS "building foxtrot client component")
    ExternalProject_Add(foxtrot_client SOURCE_DIR ${CMAKE_SOURCE_DIR}/client
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                        DEPENDS foxtrot_core)
endif()


if(BUILD_PROTOCOLS)
    message(STATUS "building foxtrot protocol library")
    ExternalProject_Add(foxtrot_protocols SOURCE_DIR ${CMAKE_SOURCE_DIR}/protocols
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                        DEPENDS foxtrot_server)
endif()

if(BUILD_DEVICES)
    message(STATUS "building foxtrot device drivers")
    
    ExternalProject_Add(foxtrot_devices SOURCE_DIR 
    ${CMAKE_SOURCE_DIR}/devices 
                        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                        DEPENDS foxtrot_protocols)
endif()
