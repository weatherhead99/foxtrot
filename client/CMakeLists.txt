project(foxtrot_client C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(packname "client")

message(STATUS "checking for conan build...")
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  message(WARNING "conanbuildinfo detected, building using conan dependencies")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  set(FOXTROT_CLIENT_CONAN_BUILD TRUE)
  conan_basic_setup()
endif()


find_package(foxtrotCore REQUIRED)
find_package(gRPC REQUIRED)
include(FoxtrotCommonSetup)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(FOXTROT_CLIENT_CONAN_BUILD)
  foxtrot_writeout_build_directory(.builddir.info)
endif()

add_library(foxtrot_client src/client.cpp)
target_include_directories(foxtrot_client PUBLIC 
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(foxtrot_client foxtrot::foxtrot_core gRPC::grpc++)
target_include_directories(foxtrot_client PRIVATE 
$<TARGET_PROPERTY:foxtrot::foxtrot_core,INTERFACE_INCLUDE_DIRECTORIES>)


set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotClient/)

install(TARGETS foxtrot_client 
        EXPORT foxtrot_client
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

install(DIRECTORY include/foxtrot
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT devel)

install(EXPORT foxtrot_client
        DESTINATION ${ft_cmakedir}
        COMPONENT devel
        NAMESPACE foxtrot::)

foxtrot_create_package_config(cmake/foxtrotClientConfig.cmake.in ${ft_cmakedir} ft_cmakedir)
foxtrot_add_to_package_registry(foxtrot_client foxtrotClient)
