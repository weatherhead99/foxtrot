cmake_minimum_required(VERSION 3.25)
project(foxtrot_core C CXX)

set(CMAKE_CXX_STANDARD 20)
#in case of building static libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/)

#import other cmake functionality we need later
include(FoxtrotCommonSetup)
include(GitVersion)

option(BUILD_UDEV_SUPPORT "Build UDEV support library (requires libudev)" ON)
option(BUILD_AVAHI_SUPPORT "Build Avahi support library (requires avahi-client)" ON)


set(packname "core")
configure_file(${CMAKE_SOURCE_DIR}/share/version.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
configure_file(${CMAKE_SOURCE_DIR}/share/version.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/${packname}_version.h)

message("foxtrot core version: ${VERSION}")

if(WIN32)
#need boost::thread on windows
find_package(Boost REQUIRED COMPONENTS log program_options thread NO_MODULE)
else()
find_package(Boost REQUIRED COMPONENTS log program_options NO_MODULE)
endif()
find_package(Protobuf REQUIRED)
find_package(rttr 0.9.6 REQUIRED)
find_package(gRPC REQUIRED)

#firs we will build the protobuf generated stuff
add_subdirectory(proto)


#our generated cmake find files will be here, just for this build
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(srcs "")
add_library(foxtrot_core)


#configure optional dependencies, generating cmake find modules if necessary
if(BUILD_UDEV_SUPPORT)
  foxtrot_generate_pkg_config_find_module(${packname} libudev)
  find_package(libudev REQUIRED)
  #extra sources for udev
  file(GLOB udev_srcs CONFIGURE_DEPENDS src/udev/*.cpp src/udev/*.cc)
  message(STATUS "udev_srcs: ${udev_srcs}")
  target_sources(foxtrot_core PRIVATE ${udev_srcs})
  target_link_libraries(foxtrot_core PUBLIC libudev::libudev)
endif()
if(BUILD_AVAHI_SUPPORT)
  foxtrot_generate_pkg_config_find_module(${packname} avahi-client)
  find_package(avahi-client REQUIRED)
endif()

#build foxtrot core library (errors and logging)
file(GLOB srcs CONFIGURE_DEPENDS src/*.cpp src/*.cc)
file(GLOB hdrs CONFIGURE_DEPENDS include/foxtrot/*.h include/foxtrot/*.hpp include/foxtrot/*.hh)

target_sources(foxtrot_core PUBLIC FILE_SET HEADERS FILES ${hdrs} BASE_DIRS include)
target_sources(foxtrot_core PRIVATE ${srcs} ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)


#foxtrot_standard_include_dirs(foxtrot_core)
foxtrot_generate_export_header(${packname} foxtrot_core)
target_include_directories(foxtrot_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)


target_link_libraries(foxtrot_core PUBLIC foxtrot_protos Boost::log Boost::program_options
  Boost::filesystem protobuf::libprotobuf
  RTTR::Core)

add_subdirectory(devprogs)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotCore/)

install(TARGETS foxtrot_core
  EXPORT foxtrot_core
  PUBLIC_HEADER
  COMPONENT devel
  FILE_SET HEADERS
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/
        COMPONENT devel)

install(EXPORT foxtrot_core
        DESTINATION ${ft_cmakedir}
        COMPONENT devel
        NAMESPACE foxtrot::)

set(ft_core_deps_script
"include(CMakeFindDependencyMacro)
 find_dependency(Boost REQUIRED COMPONENTS system log program_options filesystem thread date_time)
find_dependency(Protobuf REQUIRED)
find_dependency(gRPC REQUIRED)
find_dependency(rttr 0.9.6 REQUIRED)

#WARNING: hack to make it work on windows
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()
"
)

if(TARGET libudev::libudev)
  string(APPEND ft_core_deps_script
    "
   find_dependency(libudev REQUIRED)
")

endif()

foxtrot_setup_cmake_package(foxtrotCore foxtrot_core core ${ft_core_deps_script})

#install cmake macro files
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/FoxtrotCommonSetup.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake.in
        ${CMAKE_CURRENT_LIST_DIR}/cmake/Findlibusb.cmake
	${CMAKE_CURRENT_LIST_DIR}/cmake/Findlibudev.cmake
        DESTINATION ${ft_cmakedir})

install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/foxtrotGenericConfig.cmake.in
	DESTINATION ${ft_cmakedir}
	COMPONENT devel)


