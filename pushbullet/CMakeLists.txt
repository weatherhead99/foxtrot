find_path(rapidjson_include "prettywriter.h" PATH_SUFFIXES rapidjson)
if(NOT ${rapidjson_include})
    message(ERROR "couldn't find rapidjson path!")
else()
    message("found rapidjson: ${rapidjson_include}")
endif()
message("rapidjson_path: ${rapidjson_include}")


add_library(pushbullet OBJECT pushbullet_api.cpp)   
target_include_directories(pushbullet PUBLIC ${rapidjson_include})
