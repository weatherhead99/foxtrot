set(setups testbench_archon_heater.cpp dummy_setup.cpp)


foreach(setup ${setups})
get_filename_component(setup_we ${setup} NAME_WE)
add_library(${setup_we} MODULE ${setup})
set_target_properties(${setup_we} PROPERTIES PREFIX "")

target_include_directories(${setup_we} PRIVATE
$<TARGET_PROPERTY:foxtrotserv,INTERFACE_INCLUDE_DIRECTORIES> ${CMAKE_SOURCE_DIR}/server/ ${CMAKE_BINARY_DIR}/server/)

target_link_libraries(${setup_we} archon dummy TPG362 newport2936R)

#needed so we have already generated the protobuf files
add_dependencies(${setup_we} foxtrotserv)

endforeach()

set(TEST_SETUP_FILE ${CMAKE_CURRENT_BINARY_DIR}/testbench_archon_heater.so)
# set(TEST_SETUP_FILE ${CMAKE_CURRENT_BINARY_DIR}/testbench_archon_heater.so)
configure_file(setuptest.cpp.in setuptest.cpp)

add_executable(setuptest ${CMAKE_CURRENT_BINARY_DIR}/setuptest.cpp setuptest.cpp.in)

target_include_directories(setuptest PRIVATE
$<TARGET_PROPERTY:foxtrotserv,INTERFACE_INCLUDE_DIRECTORIES> ${CMAKE_SOURCE_DIR}/server/ ${CMAKE_BINARY_DIR}/server/)

target_link_libraries(setuptest foxtrot foxtrotserv)


