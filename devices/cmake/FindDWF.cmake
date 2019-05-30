
find_library(DWF_LIBRARY NAMES dwf)
find_path(DWF_INCLUDE NAMES dwf.h PATH_SUFFIXES digilent/waveforms digilent)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DWF DEFAULT_MSG DWF_LIBRARY DWF_INCLUDE)

add_library(DWF::DWF UNKNOWN IMPORTED)
set_target_properties(DWF::DWF PROPERTIES IMPORTED_LOCATION ${DWF_LIBRARY})
set_target_properties(DWF::DWF PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${DWF_INCLUDE})
