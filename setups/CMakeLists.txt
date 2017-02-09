add_library(testbench_archon_heater MODULE testbench_archon_heater.cpp)

target_include_directories(testbench_archon_heater PRIVATE
$<TARGET_PROPERTY:foxtrotserv,INTERFACE_INCLUDE_DIRECTORIES> ${CMAKE_SOURCE_DIR}/server/ ${CMAKE_BINARY_DIR}/server/)
