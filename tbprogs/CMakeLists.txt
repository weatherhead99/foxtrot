add_executable(tbcli tbcli.cpp tbcli_heater_logic.cpp tbcli_pressure_logic.cpp)
target_link_libraries(tbcli foxtrot foxtrotclient foxtrotserv ${Boost_PROGRAM_OPTIONS_LIBRARY})
