add_library(TPG362 TPG362.cpp)
target_link_libraries(TPG362 foxtrot)

add_executable(checksumtest test_checksum.cpp)
add_executable(tpgtest TPG362_play.cpp)

target_link_libraries(tpgtest foxtrot TPG362 )