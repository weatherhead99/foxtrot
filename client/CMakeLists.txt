cmake_minimum_required(VERSION 3.25)
project(foxtrot_client C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(packname "client")


find_package(foxtrotCore REQUIRED CONFIG)
find_package(gRPC REQUIRED)
include(FoxtrotCommonSetup)

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
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

install(DIRECTORY include/foxtrot
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT devel)

set(ft_client_deps_script "
find_dependency(foxtrotCore REQUIRED)
find_dependency(gRPC REQUIRED)

")

foxtrot_standard_setup_cmake_package(client ${ft_client_deps_script}) 
