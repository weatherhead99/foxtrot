cmake_minimum_required(VERSION 3.25)
project(foxtrot_devices C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(packname "devices")

find_package(Boost REQUIRED COMPONENTS date_time NO_MODULE)
find_package(foxtrotCore REQUIRED)
find_package(foxtrotProtocols REQUIRED)
include(FoxtrotCommonSetup)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

function(ft_device_lib_configure tgtname)
  set(options NO_STANDARD_LINK)
  cmake_parse_arguments(olib "${options}" "" "" ${ARGN})

  if(olib_NO_STANDARD_LINK)
    message(DEBUG "omitting standard link libraries")
  else()
    target_link_libraries(${tgtname} PUBLIC RTTR::Core foxtrot::foxtrot_core foxtrot::foxtrot_protocols)
  endif()
    target_link_libraries(foxtrot_devices PRIVATE ${tgtname})
    target_include_directories(foxtrot_devices PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
    install(TARGETS ${tgtname}
      EXPORT foxtrot_devices
      PUBLIC_HEADER
      COMPONENT devel
      FILE_SET HEADERS
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/devices)
endfunction()

set(ft_devices_dep_str "
find_dependency(foxtrotCore REQUIRED)
find_dependency(foxtrotProtocols REQUIRED)
find_dependency(rttr 0.9.6 REQUIRED)
"
  )

add_library(foxtrot_devices)
target_include_directories(foxtrot_devices PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/devices>)  

add_subdirectory(archon)
add_subdirectory(cornerstone_260)
add_subdirectory(OPS-Q250)
add_subdirectory(Thorlabs_APT)
add_subdirectory(newport_2936R)
add_subdirectory(Pfeiffer)
add_subdirectory(DM3068)
add_subdirectory(coldvision-ls)
add_subdirectory(ssc_chiller)
add_subdirectory(arroyo_peltier)


message(DEBUG "ft_devices_dep_str: ${ft_devices_dep_str}")

add_subdirectory(aravis_camera)

message(DEBUG "ft_devices_dep_str: ${ft_devices_dep_str}")

if(BUILD_I2C)
  add_subdirectory(BME280)
endif()
add_subdirectory(stellarnet)
add_subdirectory(webswitch_plus)
add_subdirectory(digitaldiscovery)

find_package(ueye)

if(ueye_FOUND)
  add_subdirectory(idscamera)
  add_executable(idscamera_play idscamera/idscamera_play.cpp)
  target_link_libraries(idscamera_play PUBLIC foxtrot_devices)
  string(APPEND ft_devices_dep_str
    "
      find_dependency(ueye REQUIRED)
    ")
endif()
  
add_subdirectory(DLP-IOR4)
add_subdirectory(RLY-8)

install(TARGETS foxtrot_devices 
        EXPORT foxtrot_devices)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotDevices)
install(FILES cmake/FindDWF.cmake 
 DESTINATION ${ft_cmakedir})

message(DEBUG "ft_devices_dep_str: ${ft_devices_dep_str}")

foxtrot_standard_setup_cmake_package(devices ${ft_devices_dep_str})


