add_library(arroyo_peltier OBJECT arroyo5305.cpp)
target_link_libraries(arroyo_peltier PUBLIC Boost::date_time)
target_sources(arroyo_peltier PUBLIC FILE_SET HEADERS FILES arroyo5305.hh)
ft_device_lib_configure(arroyo_peltier)


add_subdirectory(devprogs)
