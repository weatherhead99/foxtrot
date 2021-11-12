add_library(ft_server_streams OBJECT StreamThreadManager.cpp StreamThread.cpp)
target_include_directories(ft_server_streams PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(ft_server_streams PUBLIC foxtrot::foxtrot_core)


add_subdirectory(devprogs)
