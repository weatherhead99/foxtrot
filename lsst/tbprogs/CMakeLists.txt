add_executable(tbcli tbcli.cpp tbcli_heater_logic.cpp tbcli_pressure_logic.cpp tbcli_mono_logic.cpp tbcli_fill_logic.cpp)
target_link_libraries(tbcli foxtrot::foxtrot_core foxtrot::foxtrot_client  ${Boost_PROGRAM_OPTIONS_LIBRARY})

install(TARGETS tbcli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
