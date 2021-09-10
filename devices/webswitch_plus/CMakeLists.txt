find_package(rapidxml REQUIRED)

message("rapidxml: ${rapidxml_INCLUDE_DIRS}")
get_target_property(var rapidxml::rapidxml INTERFACE_INCLUDE_DIRECTORIES)
  message("var: ${var}")


add_device_object_lib(NAME webswitch_plus SRCS webswitch_plus.cpp
  INCLUDES webswitch_plus.h)

target_link_libraries(webswitch_plus PUBLIC rapidxml::rapidxml)


# add_executable(webswitch_play webswitch_play.cpp)
# target_link_libraries(webswitch_play foxtrot webswitch_plus)
