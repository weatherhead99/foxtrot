add_executable(bsc_play bsc_play.cpp)
target_link_libraries(bsc_play PUBLIC foxtrot::foxtrot_core
  foxtrot::foxtrot_protocols Thorlabs_APT)

add_executable(thorlabs_ident_play thorlabs_ident_play.cpp)
target_link_libraries(thorlabs_ident_play foxtrot::foxtrot_core
  foxtrot::foxtrot_protocols Thorlabs_APT)

add_executable(tim_play tim_play.cpp)
target_link_libraries(tim_play foxtrot::foxtrot_core
  foxtrot::foxtrot_protocols Thorlabs_APT)
