add_library(cornerstone260 cornerstone260.cpp)
target_link_libraries(cornerstone260 foxtrot)


add_executable(cornerstone_play cornerstone_play.cpp)
add_executable(setwave setwave.cpp)

target_link_libraries(cornerstone_play foxtrot cornerstone260)
target_link_libraries(setwave foxtrot cornerstone260)
