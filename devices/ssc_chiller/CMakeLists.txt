add_library(ssc_chiller OBJECT ssc_uc160.cpp ssc_tcube_edge.cpp)
target_link_libraries(ssc_chiller PUBLIC foxtrot::foxtrot_core foxtrot::foxtrot_protocols)
target_include_directories(ssc_chiller PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ft_devices_exports)

install(FILES ssc_tcube_edge.hh ssc_uc160.hh DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ft_devices_exports)

add_subdirectory(devprogs)

