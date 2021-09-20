project(foxtrot_client C CXX)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(packname "client")

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

find_package(foxtrotCore REQUIRED CONFIG)
find_package(gRPC REQUIRED)
include(FoxtrotCommonSetup)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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
