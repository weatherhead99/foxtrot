
add_device_lib(BSC203 BSC203.cpp)

add_executable(bsc_play bsc_play.cpp)
target_link_libraries(bsc_play BSC203 foxtrot)
