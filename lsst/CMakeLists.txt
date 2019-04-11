project(foxtrotlsst C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)

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
