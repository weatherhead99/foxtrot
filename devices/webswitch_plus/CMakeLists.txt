find_path(rapidxml_include "rapidxml.hpp")
if(NOT ${rapidxml_include})
    message(ERROR "couldn't find rapidxml path!")
else()
    message("found rapidxml: ${rapidxml_include}")
endif()


add_device_lib(webswitch_plus webswitch_plus.cpp)
target_include_directories(webswitch_plus PRIVATE ${rapidxml_include})

add_executable(webswitch_play webswitch_play.cpp)
target_link_libraries(webswitch_play foxtrot webswitch_plus)
