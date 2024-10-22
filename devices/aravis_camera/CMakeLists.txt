
#Find libaravis via foxtrot's internal pkg-config mechanism
foxtrot_generate_pkg_config_find_module(devices aravis-0.8)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
find_package(aravis-0.8)


if(aravis-0.8_FOUND)
  message(NOTICE "aravis found, building aravis_camera driver")
  add_library(aravis_camera OBJECT aravis_camera.cpp)
  target_sources(aravis_camera PUBLIC FILE_SET HEADERS FILES aravis_camera.hh)
  target_link_libraries(aravis_camera PUBLIC aravis-0.8::aravis-0.8)

  ft_device_lib_configure(aravis_camera)

  #note since device object libraries are PRIVATE linked into foxtrotDevices,
  #need to explicitly put a public dependency into the interface set here
  target_link_libraries(foxtrot_devices INTERFACE aravis-0.8::aravis-0.8)


  #file(COPY "lucid_genicam_dump.xml" DESTINATION ${CMAKE_BINARY_DIR})

  string(CONCAT ft_devices_dep_str_new ${ft_devices_dep_str}
      "
find_dependency(aravis-0.8 REQUIRED)
    ")

    set(ft_devices_dep_str ${ft_devices_dep_str_new} PARENT_SCOPE)


  add_subdirectory(devprogs)
else()
  message(INFO "lib aravis not found, not building aravis_camera driver")
endif()

