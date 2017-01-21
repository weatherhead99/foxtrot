add_library(DM3068 DM3068.cpp)
target_link_libraries(DM3068 foxtrot)

add_executable(DM3068_play DM3068_play.cpp)
target_link_libraries(DM3068_play foxtrot DM3068)
