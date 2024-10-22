
add_library(aravis_camera OBJECT aravis_camera.cpp)
target_link_libraries(aravis_camera PUBLIC foxtrot::foxtrot_core
  foxtrot::foxtrot_protocols PkgConfig::ARAVIS)

file(COPY "lucid_genicam_dump.xml" DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(aravis_camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(devprogs)

