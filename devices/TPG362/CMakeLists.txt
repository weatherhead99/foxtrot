add_device_lib(TPG362 TPG362.cpp)


add_executable(checksumtest test_checksum.cpp)
add_executable(tpgtest TPG362_play.cpp)

target_link_libraries(tpgtest foxtrot TPG362 )