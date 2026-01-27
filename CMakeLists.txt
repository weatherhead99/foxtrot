project(foxtrot C CXX)
cmake_minimum_required(VERSION 3.10)

include(ExternalProject)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/dist 
    CACHE PATH "installation prefix" FORCE)


macro(build_local_ft_component compname depends)
  message(STATUS "building foxtrot component ${compname}")
  ExternalProject_Add(foxtrot_${compname}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/${compname}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX} DEPENDS ${depends})
endmacro()

build_local_ft_component(core "")
build_local_ft_component(protocols foxtrot_core)
build_local_ft_component(devices foxtrot_protocols)
build_local_ft_component(client foxtrot_core)
build_local_ft_component(server foxtrot_core;foxtrot_protocols)



