find_path(rapidjson_include "prettywriter.h" PATH_SUFFIXES rapidjson)
if(NOT ${rapidjson_include})
    message(ERROR "couldn't find rapidjson path!")
else()
    message("found rapidjson: ${rapidjson_include}")
endif()
message("rapidjson_path: ${rapidjson_include}")


add_library(pushbullet OBJECT pushbullet_api.cpp)   
target_include_directories(pushbullet PUBLIC ${rapidjson_include})

add_executable(pushbullet_play pushbullet_play.cpp)
target_link_libraries(pushbullet_play foxtrot)
