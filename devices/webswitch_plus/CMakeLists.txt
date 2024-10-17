add_library(webswitch_plus OBJECT webswitch_plus.cpp)
target_sources(webswitch_plus PUBLIC FILE_SET HEADERS FILES webswitch_plus.h)

target_link_libraries(webswitch_plus PUBLIC Boost::headers)
ft_device_lib_configure(webswitch_plus)


# add_executable(webswitch_play webswitch_play.cpp)
# target_link_libraries(webswitch_play foxtrot webswitch_plus)
