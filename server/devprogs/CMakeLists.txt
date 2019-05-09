
add_executable(auth_play auth_play.cpp)
target_link_libraries(auth_play foxtrot_server)

#TODO: should be fixed somehow
#target_link_libraries(auth_play Boost::program_options)
