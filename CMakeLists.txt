project(foxtrot C CXX)
cmake_minimum_required(VERSION 3.6)

include(ExternalProject)

option(BUILD_SERVER "build foxtrot server component" ON)
option(BUILD_CLIENT "build foxtrot client component" ON)

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
