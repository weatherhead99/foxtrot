add_library(n52 n52.cpp)
add_executable(n52_play n52_play.cpp)

target_link_libraries(n52_play n52 foxtrot)
