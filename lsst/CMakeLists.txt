project(foxtrotlsst C CXX)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)

set(CONAN_CMAKE_SILENT_OUTPUT TRUE)

find_package(foxtrotCore REQUIRED CONFIG)
find_package(foxtrotClient REQUIRED CONFIG)
find_package(foxtrotProtocols REQUIRED CONFIG)
find_package(foxtrotDevices REQUIRED CONFIG)
find_package(Boost REQUIRED COMPONENTS program_options date_time)

option(MAGIS_CHILLER "include chiller setup for MAGIS experiments" OFF)


add_subdirectory(autofilld)
add_subdirectory(tbprogs)

set(setup_srcs testbench_archon_heater.cpp testbench_setup_funcs.cpp chiller_setup_funcs.cpp)
add_library(lsst_testbench SHARED ${setup_srcs})
if(${MAGIS_CHILLER})
  target_compile_definitions(lsst_testbench PRIVATE -DMAGIS_CHILLER)
endif()

target_link_libraries(lsst_testbench foxtrot::foxtrot_core 
foxtrot::foxtrot_devices)
set_target_properties(lsst_testbench PROPERTIES PREFIX "")

if(${MAGIS_CHILLER})
  message(WARNING "prefix path ${CMAKE_PREFIX_PATH}")
  find_package(foxtrotMagis REQUIRED)
  target_link_libraries(lsst_testbench foxtrot::magis_foxtrot_devices)
endif()




#TODO: fix this, side effect of new conan build!
target_include_directories(lsst_testbench PUBLIC ${CONAN_FOXTROT_DEVICES_ROOT}/include)
find_package(rapidxml REQUIRED)
target_link_libraries(lsst_testbench rapidxml::rapidxml)


option(BUILD_VCPLAY "build vacuum pump test program" OFF)

if(BUILD_VCPLAY)
add_executable(vacuum_pump_play vacuum_pump_play.cpp)
target_link_libraries(vacuum_pump_play foxtrot::foxtrot_devices)
target_include_directories(vacuum_pump_play PUBLIC ${CONAN_FOXTROT_DEVICES_ROOT}/include)

endif()


#only build if on idscamera enabled system

set(motor_setup_srcs motor_test_setup.cpp)
add_library(motor_test SHARED ${motor_setup_srcs})

if("idscamera" IN_LIST FOXTROT_DEVICES_BUILTIN)
  message(STATUS "ids camera support enabled")
  target_compile_definitions(motor_test PUBLIC -DIDS_CAMERA_ENABLED=1)
endif()

target_include_directories(motor_test PUBLIC ${CONAN_FOXTROT_DEVICES_ROOT}/include)
target_link_libraries(motor_test foxtrot::foxtrot_core foxtrot::foxtrot_devices)
set_target_properties(motor_test PROPERTIES PREFIX "")

#NOTE: disable fsmd for now as it's not working
#add_subdirectory(fsmd)


include(GNUInstallDirs)

install(TARGETS autofilld RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS tbcli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS lsst_testbench LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if("idscamera" IN_LIST FOXTROT_DEVICES_BUILTIN)
    install(TARGETS motor_test LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
