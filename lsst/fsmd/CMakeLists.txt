
add_executable(fsmd_play fsmd_play.cpp src/vacuum_system_fsm.cpp src/cryo_system_fsm.cpp)
target_include_directories(fsmd_play PRIVATE include include/foxtrot)

target_link_libraries(fsmd_play foxtrot::foxtrot_core)
