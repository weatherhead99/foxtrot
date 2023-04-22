project(foxtrot_server C CXX)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CONAN_CMAKE_SILENT_OUTPUT ON)

set(packname "server")


find_package(foxtrotCore REQUIRED CONFIG)
find_package(foxtrotProtocols REQUIRED CONFIG)

find_package(rttr 0.9.6 REQUIRED)
message("rttr version: ${rttr_VERSION}")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

find_package(Boost REQUIRED COMPONENTS program_options)

find_package(RapidJSON REQUIRED)
find_package(libsodium REQUIRED)


if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    find_library(dl dl)
    message(STATUS "libdl: ${dl}")
else()
    message(STATUS "windows build, don't need libdl")
    set(dl "")
endif()


include(GenerateExportHeader)
include(GNUInstallDirs)

message("module dir: ${CMAKE_MODULE_PATH}")
include(FoxtrotCommonSetup)


add_subdirectory(src)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotServer/)

install(TARGETS exptserve
       EXPORT foxtrot_server
       RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(TARGETS ftauth
           RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


install(TARGETS foxtrot_server
       EXPORT foxtrot_server
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
       ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)


install(EXPORT foxtrot_server
       DESTINATION ${ft_cmakedir}
       COMPONENT devel
       NAMESPACE foxtrot::)

install(DIRECTORY include/foxtrot
       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
       COMPONENT devel)
     

install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/Findsodium.cmake DESTINATION ${ft_cmakedir})

set(ft_server_dep_str
"
find_dependency(foxtrotCore REQUIRED)
find_dependency(Boost REQUIRED COMPONENTS program_options filesystem)
find_dependency(CURL REQUIRED)
find_dependency(RapidJSON REQUIRED MODULE)
find_dependency(libsodium REQUIRED)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    find_library(dl dl)
    message(STATUS "libdl: ${dl}")
else()
    message(STATUS "windows build, don't need libdl")
    set(dl "")
endif()
")

foxtrot_standard_setup_cmake_package(server ${ft_server_dep_str})


add_subdirectory(devprogs)

#build the dummy device
add_subdirectory(devices)
add_subdirectory(setups)
