project(foxtrotlsst C CXX)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
set(CONAN_CMAKE_SILENT_OUTPUT TRUE)

find_package(foxtrotCore REQUIRED CONFIG)
find_package(foxtrotClient REQUIRED CONFIG)
find_package(foxtrotProtocols REQUIRED CONFIG)
find_package(foxtrotDevices REQUIRED CONFIG)
find_package(Boost REQUIRED COMPONENTS program_options date_time)

add_subdirectory(autofilld)
add_subdirectory(tbprogs)

set(setup_srcs testbench_archon_heater.cpp testbench_setup_funcs.cpp)
add_library(lsst_testbench SHARED ${setup_srcs})
target_link_libraries(lsst_testbench foxtrot::foxtrot_core 
foxtrot::foxtrot_devices)
set_target_properties(lsst_testbench PROPERTIES PREFIX "")

#TODO: fix this, side effect of new conan build!
target_include_directories(lsst_testbench PUBLIC ${CONAN_FOXTROT_DEVICES_ROOT}/include)
find_package(rapidxml REQUIRED)
target_link_libraries(lsst_testbench rapidxml::rapidxml)



#add_executable(vacuum_pump_play vacuum_pump_play.cpp)
#target_link_libraries(vacuum_pump_play foxtrot::foxtrot_devices)

#only build if on idscamera enabled system
if("idscamera" IN_LIST FOXTROT_DEVICES_BUILTIN)
    set(motor_setup_srcs motor_test_setup.cpp)
    add_library(motor_test SHARED ${motor_setup_srcs})
    target_link_libraries(motor_test foxtrot::foxtrot_core foxtrot::foxtrot_devices)
    set_target_properties(motor_test PROPERTIES PREFIX "")
endif()

#NOTE: disable fsmd for now as it's not working
add_subdirectory(fsmd)


include(GNUInstallDirs)

install(TARGETS autofilld RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS tbcli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS lsst_testbench LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if("idscamera" IN_LIST FOXTROT_DEVICES_BUILTIN)
    install(TARGETS motor_test LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
