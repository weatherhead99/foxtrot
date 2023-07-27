#NOTE recent ueye apis started including their own cmake files

#add_device_object_lib(NAME idscamera SRCS idscamera.cpp Image.cpp INCLUDES idscamera.h Image.h
#    LINKLIBS ueye::ueye)

add_device_object_lib(NAME idscamera SRCS idscamera.cpp Image.cpp INCLUDES idscamera.h Image.h
    LINKLIBS ueye::api)

