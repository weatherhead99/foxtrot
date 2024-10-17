add_library(DM3068 OBJECT DM3068.cpp)
target_sources(DM3068 PUBLIC FILE_SET HEADERS FILES DM3068.h)

ft_device_lib_configure(DM3068)



# add_executable(DM3068_play DM3068_play.cpp)
# target_link_libraries(DM3068_play foxtrot DM3068)
