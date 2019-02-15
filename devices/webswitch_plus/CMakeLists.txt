find_path(rapidxml_include "rapidxml.hpp")
if(NOT ${rapidxml_include})
    message(ERROR "couldn't find rapidxml path!")
else()
    message("found rapidxml: ${rapidxml_include}")
endif()


add_device_lib(webswitch_plus webswitch_plus.cpp)
target_include_directories(webswitch_plus PRIVATE ${rapidxml_include})
