add_device_lib(newport2936R newport2936R.cpp)

add_executable(OPM_play OPM_play.cpp)
target_link_libraries(OPM_play newport2936R foxtrot  )

add_executable(test_conv test_conv.cpp)
target_link_libraries(test_conv newport2936R foxtrot)
