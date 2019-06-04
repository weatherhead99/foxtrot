find_path(RAPIDXML_INCLUDE NAMES "rapidxml.hpp" PATH_SUFFIXES rapidxml)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(rapidxml DEFAULT_MSG RAPIDXML_INCLUDE)

add_library(rapidxml::rapidxml INTERFACE IMPORTED GLOBAL)
set_target_properties(rapidxml::rapidxml PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
  ${RAPIDXML_INCLUDE})

