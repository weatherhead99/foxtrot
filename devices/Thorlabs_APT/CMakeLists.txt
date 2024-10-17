add_library(Thorlabs_APT OBJECT BSC203.cpp TIM101.cpp APT.cpp)
target_sources(Thorlabs_APT PUBLIC FILE_SET HEADERS FILES BSC203.h TIM101.h APT.h APT_defs.hh)

ft_device_lib_configure(Thorlabs_APT)


add_executable(thorlabs_ident_play thorlabs_ident_play.cpp)
target_link_libraries(thorlabs_ident_play Thorlabs_APT)
# add_executable(bsc_play bsc_play.cpp)
# target_link_libraries(bsc_play BSC203 foxtrot)

add_executable(tim_play tim_play.cpp)
target_link_libraries(tim_play PUBLIC Thorlabs_APT)

add_executable(bsc_play bsc_play.cpp)
target_link_libraries(bsc_play PUBLIC Thorlabs_APT)
