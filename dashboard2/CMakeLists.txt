qt5_wrap_ui(dashboard2_forms_headers dashboard2.ui)
add_executable(dashboard2 dashboard2.cpp  dashboard2_main.cpp ${dashboard2_forms_headers})

target_link_libraries(dashboard2 Qt5::Widgets  ${HOST_SPEC_LIBRARIES} OPMD )
target_include_directories(dashboard2 PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${QWT_INCLUDE_DIRS})