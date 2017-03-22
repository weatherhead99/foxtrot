
qt5_wrap_ui(dashboard_forms_headers dashboard_main.ui)
add_executable(dashboard dashboard.cpp dashboard_main.cpp ${dashboard_forms_headers})
target_link_libraries(dashboard Qt5::Widgets ${QWT_LIBRARIES} foxtrotclient ${HOST_SPEC_LIBRARIES})
find_library(png16 png16) 


target_include_directories(dashboard PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${QWT_INCLUDE_DIRS} )
