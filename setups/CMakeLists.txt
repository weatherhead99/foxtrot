set(setups testbench_archon_heater.cpp dummy_setup.cpp)

include(GenerateExportHeader)
macro(build_setup setupname srcfiles libs)
add_library(${setupname} MODULE ${srcfiles})
set_target_properties(${setupname} PROPERTIES PREFIX "")
target_compile_definitions(${setupname} PRIVATE -D ${setupname}_EXPORTS)
target_include_directories(${setupname} PRIVATE 
$<TARGET_PROPERTY:foxtrotserv,INTERFACE_INCLUDE_DIRECTORIES> ${CMAKE_SOURCE_DIR}/server/ ${CMAKE_BINARY_DIR}/server/ ${CMAKE_CURRENT_BINARY_DIR})
add_dependencies(${setupname} foxtrotserv)
install(TARGETS ${setupname} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/setups/)
target_link_libraries(${setupname} PUBLIC ${libs})

generate_export_header(${setupname})

endmacro()



if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	build_setup(testbench_archon_heater "testbench_archon_heater.cpp;testbench_setup_funcs.cpp" "archon;TPG362;newport2936R;cornerstone260;Q250;stellarnet")
	build_setup(dummy_setup dummy_setup.cpp dummy)
else()
	build_setup(testbench_archon_heater "testbench_archon_heater.cpp;testbench_setup_funcs.cpp" "archon;TPG362;newport2936R;cornerstone260;stellarnet;${RTTR_TARGET};foxtrot;${Boost_DATE_TIME_LIBRARY};foxtrotserv")
	build_setup(dummy_setup dummy_setup.cpp "dummy;foxtrot;foxtrotserv")
endif()



set(TEST_SETUP_FILE ${CMAKE_CURRENT_BINARY_DIR}/testbench_archon_heater.so)
# set(TEST_SETUP_FILE ${CMAKE_CURRENT_BINARY_DIR}/testbench_archon_heater.so)

configure_file(setuptest.cpp.in setuptest.cpp)

add_executable(setuptest ${CMAKE_CURRENT_BINARY_DIR}/setuptest.cpp setuptest.cpp.in)

target_include_directories(setuptest PRIVATE
$<TARGET_PROPERTY:foxtrotserv,INTERFACE_INCLUDE_DIRECTORIES> ${CMAKE_SOURCE_DIR}/server/ ${CMAKE_BINARY_DIR}/server/)

target_link_libraries(setuptest foxtrot foxtrotserv)



