
add_executable(auth_play auth_play.cpp)
    target_link_libraries(auth_play foxtrot_server)

#TODO: should be fixed somehow
#target_link_libraries(auth_play Boost::program_options)

add_executable(asio_server_dev asio_server_dev.cpp)
target_link_libraries(asio_server_dev PUBLIC foxtrot::foxtrot_core
  asio-grpc::asio-grpc)

