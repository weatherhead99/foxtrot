
add_device_object_lib(NAME BSC203 SRCS BSC203.cpp TIM101.cpp APT.cpp INCLUDES BSC203.h TIM101.h APT.h)


add_executable(thorlabs_ident_play thorlabs_ident_play.cpp)
target_link_libraries(thorlabs_ident_play BSC203 foxtrot::foxtrot_core RTTR::Core foxtrot::foxtrot_protocols)
# add_executable(bsc_play bsc_play.cpp)
# target_link_libraries(bsc_play BSC203 foxtrot)
