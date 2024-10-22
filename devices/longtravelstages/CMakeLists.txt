add_library(LTS300 OBJECT LTS300.cpp APT_defaultdest.cpp M30X.cpp)
target_link_libraries(LTS300 PUBLIC foxtrot::foxtrot_devices foxtrot::foxtrot_core Boost::log)
target_include_directories(LTS300 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/foxtrot/ft_devices_exports>)

install(FILES LTS300.hh DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ft_devices_exports)

add_subdirectory(devprogs)
