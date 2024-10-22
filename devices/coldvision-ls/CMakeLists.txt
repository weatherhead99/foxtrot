add_library(coldvision_ls OBJECT coldvision_ls.cpp)
target_sources(coldvision_ls PUBLIC FILE_SET HEADERS FILES coldvision.hh)
ft_device_lib_configure(coldvision_ls)

add_subdirectory(devprogs)


