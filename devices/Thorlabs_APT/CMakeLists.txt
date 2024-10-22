file(GLOB thorlabs_srcs *.cpp)
file(GLOB thorlabs_hdrs *.hh *.h)

add_library(Thorlabs_APT OBJECT ${thorlabs_srcs})
target_sources(Thorlabs_APT PUBLIC FILE_SET HEADERS FILES ${thorlabs_hdrs})

ft_device_lib_configure(Thorlabs_APT)

add_subdirectory(devprogs)

