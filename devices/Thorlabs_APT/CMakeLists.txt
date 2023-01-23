
add_device_object_lib(NAME Thorlabs_APT SRCS BSC203.cpp TIM101.cpp APT.cpp INCLUDES BSC203.h TIM101.h APT.h)


add_executable(thorlabs_ident_play thorlabs_ident_play.cpp)
target_link_libraries(thorlabs_ident_play Thorlabs_APT foxtrot::foxtrot_core RTTR::Core foxtrot::foxtrot_protocols)
# add_executable(bsc_play bsc_play.cpp)
# target_link_libraries(bsc_play BSC203 foxtrot)

add_executable(tim_play tim_play.cpp)
target_link_libraries(tim_play PUBLIC Thorlabs_APT foxtrot::foxtrot_protocols)

add_executable(bsc_play bsc_play.cpp)
target_link_libraries(bsc_play PUBLIC Thorlabs_APT foxtrot::foxtrot_protocols)
