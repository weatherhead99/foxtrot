find_path(rapidxml_include "rapidxml.hpp" PATH_SUFFIXES rapidxml)
if(NOT ${rapidxml_include})
    message(ERROR "couldn't find rapidxml path!")
else()
    message("found rapidxml: ${rapidxml_include}")
endif()

message(WARNING "rapidxml: ${rapidxml_include}")


add_device_object_lib(NAME webswitch_plus SRCS webswitch_plus.cpp INCLUDES webswitch_plus.h)
target_include_directories(webswitch_plus PUBLIC ${rapidxml_include})

# add_executable(webswitch_play webswitch_play.cpp)
# target_link_libraries(webswitch_play foxtrot webswitch_plus)
