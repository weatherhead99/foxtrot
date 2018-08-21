add_device_lib(cornerstone260 cornerstone260.cpp)


add_executable(cornerstone_play cornerstone_play.cpp)
add_executable(setwave setwave.cpp)

target_link_libraries(cornerstone_play cornerstone260 foxtrot)
target_link_libraries(setwave foxtrot cornerstone260)
