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

find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED CONFIG)
find_package(rttr 0.9.6 REQUIRED)


#firs we will build the protobuf generated stuff
add_subdirectory(proto)


#our generated cmake find files will be here, just for this build
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(srcs "")
add_library(foxtrot_core)


#configure optional dependencies, generating cmake find modules if necessary
if(BUILD_UDEV_SUPPORT)
  message(STATUS "requested udev support...")
  foxtrot_generate_pkg_config_find_module(${packname} libudev)
  find_package(libudev REQUIRED)
  message(NOTICE "libudev found, udev enabled!")
  #extra sources for udev
  file(GLOB udev_srcs CONFIGURE_DEPENDS src/udev/*.cpp src/udev/*.cc)
  file(GLOB udev_hdrs CONFIGURE_DEPENDS include/foxtrot/udev/*.hh)
  message(STATUS "udev_srcs: ${udev_srcs}")
  message(STATUS "udev_hdrs: ${udev_hdrs}")
  target_sources(foxtrot_core PRIVATE ${udev_srcs})
  target_sources(foxtrot_core PUBLIC FILE_SET HEADERS FILES ${udev_hdrs} BASE_DIRS include)
  target_link_libraries(foxtrot_core PUBLIC libudev::libudev)
endif()
if(BUILD_AVAHI_SUPPORT)
  message(STATUS "requested avahi support...")
  foxtrot_generate_pkg_config_find_module(${packname} avahi-client)
  find_package(avahi-client REQUIRED)
  message(NOTICE "avahi-client found, avahi enabled!")
endif()

#build foxtrot core library (errors and logging)
file(GLOB srcs CONFIGURE_DEPENDS src/*.cpp src/*.cc)
file(GLOB hdrs CONFIGURE_DEPENDS include/foxtrot/*.h include/foxtrot/*.hpp include/foxtrot/*.hh)

message(DEBUG "headers: ${hdrs}")

target_sources(foxtrot_core PUBLIC FILE_SET HEADERS FILES ${hdrs} BASE_DIRS include)
target_sources(foxtrot_core PRIVATE ${srcs} ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)


#foxtrot_standard_include_dirs(foxtrot_core)
foxtrot_generate_export_header(${packname} foxtrot_core)
target_include_directories(foxtrot_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)


target_link_libraries(foxtrot_core PUBLIC foxtrot_protos Boost::log Boost::program_options
  Boost::filesystem
  RTTR::Core)

add_subdirectory(devprogs)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotCore/)

install(TARGETS foxtrot_core
  EXPORT foxtrot_core
  PUBLIC_HEADER
  COMPONENT devel
  FILE_SET HEADERS)

set(ft_core_deps_script
"
find_dependency(Boost REQUIRED COMPONENTS system log program_options filesystem thread date_time NO_MODULE)
find_dependency(gRPC REQUIRED)
find_dependency(Protobuf REQUIRED)
find_dependency(rttr 0.9.6 REQUIRED)
"
)

if(TARGET libudev::libudev)
  string(APPEND ft_core_deps_script
    "
   find_dependency(libudev REQUIRED)
")

endif()

foxtrot_standard_setup_cmake_package(core ${ft_core_deps_script})


#install cmake macro files
file(GLOB cmake_scripts cmake/*.cmake cmake/*.cmake.in)

install(FILES ${cmake_scripts}
  DESTINATION ${ft_cmakedir}
  COMPONENT devel)


