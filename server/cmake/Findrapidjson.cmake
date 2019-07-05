find_path(RAPIDJSON_INCLUDE NAMES "rapidjson.h" PATH_SUFFIXES rapidjson)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(rapidjson DEFAULT_MSG RAPIDJSON_INCLUDE)

add_library(rapidjson::rapidjson INTERFACE IMPORTED GLOBAL)
set_target_properties(rapidjson::rapidjson PROPERTIES 
            INTERFACE_INCLUDE_DIRECTORIES ${RAPIDJSON_INCLUDE})
