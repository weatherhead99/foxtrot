add_executable(tbcli tbcli.cpp)
target_link_libraries(tbcli foxtrotclient ${Boost_PROGRAM_OPTIONS_LIBRARY})
