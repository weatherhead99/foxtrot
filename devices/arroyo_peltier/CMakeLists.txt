find_package(Boost REQUIRED COMPONENTS date_time)

add_library(arroyo_peltier OBJECT arroyo5305.cpp)
target_link_libraries(arroyo_peltier PUBLIC foxtrot::foxtrot_core foxtrot::foxtrot_protocols
Boost::date_time)
target_include_directories(arroyo_peltier PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>$<INSTALL_INTERFACE:include/foxtrot/ft_devices_exports>)


install(FILES arroyo5305.hh DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ft_devices_exports)

add_subdirectory(devprogs)
