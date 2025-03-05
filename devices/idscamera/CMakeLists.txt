#NOTE recent ueye apis started including their own cmake files

#add_device_object_lib(NAME idscamera SRCS idscamera.cpp Image.cpp INCLUDES idscamera.h Image.h
#    LINKLIBS ueye::ueye)

add_library(idscamera OBJECT idscamera.cpp Image.cpp)
target_sources(idscamera PUBLIC FILE_SET HEADERS FILES idscamera.h Image.h)
target_link_libraries(idscamera PUBLIC ueye::api)
ft_device_lib_configure(idscamera)
