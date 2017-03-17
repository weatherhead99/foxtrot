find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
get_target_property(qcore Qt5::Core  LOCATION_Release)
message(STATUS "qt: ${qcore}")

find_package(Qwt REQUIRED)

set(CMAKE_AUTOMOC ON)

qt5_wrap_ui(dashboard_forms_headers dashboard_main.ui)
add_executable(dashboard dashboard.cpp dashboard_main.cpp ${dashboard_forms_headers})
target_link_libraries(dashboard Qt5::Widgets ${QWT_LIBRARIES} foxtrotclient)
target_include_directories(dashboard PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${QWT_INCLUDE_DIRS} )
