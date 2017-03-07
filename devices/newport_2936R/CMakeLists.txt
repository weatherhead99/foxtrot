add_device_lib(newport2936R newport2936R.cpp)

add_executable(OPM_play OPM_play.cpp)
target_link_libraries(OPM_play newport2936R foxtrot  )
