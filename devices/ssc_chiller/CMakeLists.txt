add_library(ssc_chiller OBJECT ssc_uc160.cpp ssc_tcube_edge.cpp)
target_sources(ssc_chiller PUBLIC FILE_SET HEADERS FILES ssc_tcube_edge.hh ssc_uc160.hh)
ft_device_lib_configure(ssc_chiller)
add_subdirectory(devprogs)

