add_library(coldvision_ls OBJECT coldvision_ls.cpp)
target_link_libraries(coldvision_ls PUBLIC foxtrot::foxtrot_core foxtrot::foxtrot_protocols)

target_include_directories(coldvision_ls PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/foxtrot/ft_devices_exports>)

install(FILES coldvision.hh DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ft_devices_exports)

add_subdirectory(devprogs)


