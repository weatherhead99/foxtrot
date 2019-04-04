project(foxtrot_client C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(packname "client")

find_package(foxtrotCore REQUIRED)
include(FoxtrotCommonSetup)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(foxtrot_client src/client.cpp)
target_include_directories(foxtrot_client PUBLIC 
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(foxtrot_client foxtrot::foxtrot_core)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotServer/)

install(TARGETS foxtrot_client 
        EXPORT foxtrot_client
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

install(DIRECTORY include/foxtrot
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT devel)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/)
install(EXPORT foxtrot_client
        DESTINATION ${ft_cmakedir}
        COMPONENT devel
        NAMESPACE foxtrot::)

configure_package_config_file(cmake/foxtrotClientConfig.cmake.in
                              foxtrotClientConfig.cmake
                              INSTALL_DESTINATION ${ft_cmakedir}
                              PATH_VARS ft_cmakedir)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrotClientConfig.cmake
        DESTINATION ${ft_cmakedir}
        COMPONENT devel)
