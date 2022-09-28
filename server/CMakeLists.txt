project(foxtrot_server C CXX)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CONAN_CMAKE_SILENT_OUTPUT ON)

set(packname "server")


find_package(foxtrotCore REQUIRED CONFIG)

get_target_property(prop foxtrot::foxtrot_core INTERFACE_INCLUDE_DIRECTORIES)
message(ERROR "iid: ${prop}")


find_package(OpenSSL REQUIRED)
find_package(rttr 0.9.6 REQUIRED)
message("rttr version: ${rttr_VERSION}")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

find_package(Boost REQUIRED COMPONENTS program_options filesystem)
find_package(CURL REQUIRED MODULE)


find_package(RapidJSON REQUIRED MODULE)
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
include(CMakePackageConfigHelpers)

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

set(ft_server_sourcedir ${CMAKE_CURRENT_SOURCE_DIR})

foxtrot_create_package_config(cmake/foxtrotServerConfig.cmake.in
                               ${ft_cmakedir}
                               "ft_cmakedir;ft_server_sourcedir")

foxtrot_add_to_package_registry(foxtrot_server foxtrotServer)

add_subdirectory(devprogs)

#build the dummy device
add_subdirectory(devices)
add_subdirectory(setups)
