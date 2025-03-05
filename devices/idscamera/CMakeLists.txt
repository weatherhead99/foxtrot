#NOTE recent ueye apis started including their own cmake files

#add_device_object_lib(NAME idscamera SRCS idscamera.cpp Image.cpp INCLUDES idscamera.h Image.h
#    LINKLIBS ueye::ueye)

find_package(ueye)

if(ueye_FOUND)

  add_library(idscamera OBJECT idscamera.cpp Image.cpp)
  target_sources(idscamera PUBLIC FILE_SET HEADERS FILES idscamera.h Image.h)
  target_link_libraries(idscamera PUBLIC ueye::api)
  ft_device_lib_configure(idscamera)

  add_executable(idscamera_play idscamera_play.cpp)
  target_link_libraries(idscamera_play PUBLIC idscamera)
  string(APPEND ft_devices_dep_str
    "
      find_dependency(ueye REQUIRED)
    ")

endif()
