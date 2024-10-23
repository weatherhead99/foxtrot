cmake_minimum_required(VERSION 3.25)
project(foxtrotlsst C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CONAN_CMAKE_SILENT_OUTPUT TRUE)

find_package(foxtrotClient REQUIRED CONFIG)
find_package(foxtrotDevices REQUIRED CONFIG)
find_package(Boost REQUIRED COMPONENTS program_options date_time system NO_MODULE)
include(FoxtrotCommonSetup)


add_subdirectory(autofilld)
add_subdirectory(tbprogs)


foxtrot_build_setup_file("testbench_archon_heater.cpp;testbench_setup_funcs.cpp" TARGET_NAME lsst_testbench)
target_link_libraries(lsst_testbench PUBLIC Boost::system)


option(BUILD_VCPLAY "build vacuum pump test program" OFF)

if(BUILD_VCPLAY)
add_executable(vacuum_pump_play vacuum_pump_play.cpp)
target_link_libraries(vacuum_pump_play foxtrot::foxtrot_devices)
target_include_directories(vacuum_pump_play PUBLIC ${CONAN_FOXTROT_DEVICES_ROOT}/include)

endif()


#only build if on idscamera enabled system

foxtrot_build_setup_file(motor_test_setup.cpp TARGET_NAME motor_test)

if("idscamera" IN_LIST FOXTROT_DEVICES_BUILTIN)
  message(STATUS "ids camera support enabled")
  target_compile_definitions(motor_test PUBLIC -DIDS_CAMERA_ENABLED=1)
endif()


#NOTE: disable fsmd for now as it's not working
#add_subdirectory(fsmd)


install(TARGETS autofilld RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS tbcli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if("idscamera" IN_LIST FOXTROT_DEVICES_BUILTIN)
    install(TARGETS motor_test LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
