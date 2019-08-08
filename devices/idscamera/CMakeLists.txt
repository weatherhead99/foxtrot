find_package(ueye)

if(${ueye_FOUND})
    message(STATUS "found uEye API library, building idscamera device")
    add_device_object_lib(NAME idscamera SRCS idscamera.cpp Image.cpp INCLUDES idscamera.h Image.h
    LINKLIBS ueye::ueye)
endif()
