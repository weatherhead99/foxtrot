add_library(cornerstone260 cornerstone260.cpp)
target_link_libraries(cornerstone260 foxtrot)


add_executable(cornerstone_play cornerstone_play.cpp)


target_link_libraries(cornerstone_play foxtrot cornerstone260)

