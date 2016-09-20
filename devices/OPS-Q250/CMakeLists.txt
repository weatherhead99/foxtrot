add_library(Q250 Q250.cpp)

add_executable(Q250_play Q250_play.cpp)
target_link_libraries(Q250_play Q250 foxtrot)