project(foxtrot_devices C CXX)
cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(packname "devices")

message(STATUS "checking for conan build...")
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  message(WARNING "conanbuildinfo detected, building using conan dependencies")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  set(FOXTROT_DEVICES_CONAN_BUILD TRUE)
  conan_basic_setup()
endif()


find_package(foxtrotCore REQUIRED)
include(FoxtrotCommonSetup)
include(GNUInstallDirs)
include(CMakeParseArguments)

find_package(foxtrotProtocols REQUIRED)
find_package(rttr 0.9.6 REQUIRED)


file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/foxtrot)
set(device_targets "")

macro(add_device_object_lib)
    set(options OPTIONAL)
    set(oneValueArgs NAME)
    set(multiValueArgs SRCS INCLUDES)
    cmake_parse_arguments(olib "${options}" "${oneValueArgs}" 
            "${multiValueArgs}" ${ARGN})
            
    add_library(${olib_NAME} OBJECT ${olib_SRCS})
    target_include_directories(${olib_NAME} PRIVATE
    $<TARGET_PROPERTY:foxtrot::foxtrot_server,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:foxtrot::foxtrot_core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:foxtrot::foxtrot_protocols,INTERFACE_INCLUDE_DIRECTORIES>)
    
    list(APPEND device_targets ${olib_NAME})
    set(device_targets ${device_targets} PARENT_SCOPE)
    
    file(COPY ${olib_INCLUDES} 
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/devices)
    
    install(FILES ${olib_INCLUDES}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/devices/)
endmacro()


add_subdirectory(archon)
add_subdirectory(cornerstone_260)
add_subdirectory(OPS-Q250)
add_subdirectory(BSC203)
add_subdirectory(newport_2936R)
add_subdirectory(TPG362)
add_subdirectory(DM3068)
if(BUILD_I2C)
  add_subdirectory(BME280)
endif()
add_subdirectory(stellarnet)
add_subdirectory(webswitch_plus)

set(libobjs "")
message(STATUS "device targetS: ${device_targets}")
foreach(tgt ${device_targets})
    list(APPEND libobjs "$<TARGET_OBJECTS:${tgt}>")
endforeach()
message(STATUS "libobjs: ${libobjs}")
add_library(foxtrot_devices ${libobjs})

install(TARGETS foxtrot_devices 
        EXPORT foxtrot_devices
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)
        
set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotDevices)

install(EXPORT foxtrot_devices
        DESTINATION ${ft_cmakedir}
        NAMESPACE foxtrot::
        COMPONENT devel)
        
foxtrot_create_package_config(cmake/foxtrotDevicesConfig.cmake.in ${ft_cmakedir} 
"ft_cmakedir;device_targets")

foxtrot_add_to_package_registry(foxtrot_devices
foxtrotDevices)
