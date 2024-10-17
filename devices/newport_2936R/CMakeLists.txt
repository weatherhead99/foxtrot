add_library(newport2936R OBJECT newport2936R.cpp)
target_sources(newport2936R PUBLIC FILE_SET HEADERS FILES newport2936R.h)

ft_device_lib_configure(newport2936R)


#target_compile_definitions(newport2936R PUBLIC -DNEW_RTTR_API)

# target_link_libraries(newport2936R)
# 
# add_executable(OPM_play OPM_play.cpp)
# target_link_libraries(OPM_play newport2936R foxtrot )
# 
# add_executable(test_conv test_conv.cpp)
# target_link_libraries(test_conv newport2936R foxtrot)
