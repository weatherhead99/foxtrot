find_library(UEYE_LIBRARY NAMES ueye_api)
find_path(UEYE_INCLUDE NAMES ueye.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ueye DEFAULT_MSG UEYE_LIBRARY UEYE_INCLUDE)

add_library(ueye::ueye UNKNOWN IMPORTED GLOBAL)
set_target_properties(ueye::ueye PROPERTIES IMPORTED_LOCATION ${UEYE_LIBRARY})
set_target_properties(ueye::ueye PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${UEYE_INCLUDE})
