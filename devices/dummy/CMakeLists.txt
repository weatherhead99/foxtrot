add_library(dummy dummyDevice.cpp)

add_executable(dummy_play dummy_play.cpp)
target_link_libraries(dummy_play dummy foxtrot ${rttr})
