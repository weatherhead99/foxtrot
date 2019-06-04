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
set(link_later_libs "")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

macro(add_device_object_lib)
    set(options OPTIONAL)
    set(oneValueArgs NAME)
    set(multiValueArgs SRCS INCLUDES LINKLIBS)
    cmake_parse_arguments(olib "${options}" "${oneValueArgs}" 
            "${multiValueArgs}" ${ARGN})
            
	  add_library(${olib_NAME} OBJECT ${olib_SRCS})

    target_include_directories(${olib_NAME} PRIVATE
    $<TARGET_PROPERTY:foxtrot::foxtrot_server,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:foxtrot::foxtrot_core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:foxtrot::foxtrot_protocols,INTERFACE_INCLUDE_DIRECTORIES>)
  
    list(APPEND device_targets ${olib_NAME})
    set(device_targets ${device_targets} PARENT_SCOPE)
    
    list(APPEND link_later_libs ${olib_LINKLIBS})
    set(link_later_libs ${link_later_libs} PARENT_SCOPE)

    foreach(lib ${link_later_libs})
      target_include_directories(${olib_NAME} PUBLIC
	$<TARGET_PROPERTY:${lib},INTERFACE_INCLUDE_DIRECTORIES>)
    endforeach()
    
    set(olib_INCLUDES_abs "")
    foreach(include ${olib_INCLUDES})
      get_filename_component(fla ${include} ABSOLUTE)
      list(APPEND olib_INCLUDES_abs ${fla})
    endforeach()
    
    add_custom_target(${olib_NAME}_copy_includes
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${olib_INCLUDES_abs}
      ${CMAKE_BINARY_DIR}/foxtrot/devices/
      COMMENT "copying include tree to top level build tree")

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
add_subdirectory(digitaldiscovery)
add_subdirectory(idscamera)

set(libobjs "")
message(STATUS "device targetS: ${device_targets}")
foreach(tgt ${device_targets})
    list(APPEND libobjs "$<TARGET_OBJECTS:${tgt}>")
endforeach()
message(STATUS "libobjs: ${libobjs}")
add_library(foxtrot_devices ${libobjs})
target_link_libraries(foxtrot_devices PUBLIC foxtrot::foxtrot_protocols)

foreach(lib ${link_later_libs})
    message(STATUS "linking in requested library: ${lib}")
    target_link_libraries(foxtrot_devices PUBLIC ${lib})
endforeach()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/foxtrot/devices)
foreach(tgt ${device_targets})
  add_dependencies(foxtrot_devices ${tgt}_copy_includes)
endforeach()


install(TARGETS foxtrot_devices 
        EXPORT foxtrot_devices
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)
        
set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotDevices)

install(EXPORT foxtrot_devices
        DESTINATION ${ft_cmakedir}
        NAMESPACE foxtrot::
        COMPONENT devel)

install(FILES cmake/Findrapidxml.cmake cmake/FindDWF.cmake 
cmake/Findueye.cmake DESTINATION ${ft_cmakedir})
        
set(ft_devices_sourcedir ${CMAKE_CURRENT_SOURCE_DIR})

foxtrot_create_package_config(cmake/foxtrotDevicesConfig.cmake.in ${ft_cmakedir} 
"ft_cmakedir;device_targets;ft_devices_sourcedir")

foxtrot_add_to_package_registry(foxtrot_devices
foxtrotDevices)
