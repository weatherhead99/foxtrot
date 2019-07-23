project(foxtrotlsst C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)

message(STATUS "checking for conan build...")
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  message(WARNING "conanbuildinfo detected, building using conan dependencies")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  set(FOXTROT_CLIENT_CONAN_BUILD TRUE)
  conan_basic_setup()
endif()

find_package(foxtrotClient REQUIRED)
find_package(foxtrotProtocols REQUIRED)
find_package(foxtrotDevices REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

set(autofilld_srcs autofilld/autofilld.cpp autofilld/autofill_fill_logic.cpp
    autofilld/autofill_logger.cpp)
add_executable(autofilld ${autofilld_srcs})
target_link_libraries(autofilld foxtrot::foxtrot_client Boost::program_options)

set(tbcli_srcs tbprogs/tbcli.cpp tbprogs/tbcli_fill_logic.cpp tbprogs/tbcli_heater_logic.cpp
    tbprogs/tbcli_mono_logic.cpp tbprogs/tbcli_pressure_logic.cpp)
    
add_executable(tbcli ${tbcli_srcs})
target_link_libraries(tbcli foxtrot::foxtrot_client Boost::program_options)


set(setup_srcs testbench_archon_heater.cpp testbench_setup_funcs.cpp)
add_library(lsst_testbench SHARED ${setup_srcs})
target_link_libraries(lsst_testbench foxtrot::foxtrot_core foxtrot::foxtrot_server
foxtrot::foxtrot_devices)
set_target_properties(lsst_testbench PROPERTIES PREFIX "")


set(motor_setup_srcs motor_test_setup.cpp)
add_library(motor_test SHARED ${motor_setup_srcs})
target_link_libraries(motor_test foxtrot::foxtrot_core
foxtrot::foxtrot_server foxtrot::foxtrot_devices)
set_target_properties(motor_setup_srcs PROPERTIES PREFIX
"")


include(GNUInstallDirs)

install(TARGETS autofilld RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS tbcli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS lsst_testbench LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
