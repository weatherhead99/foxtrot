find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)

qt5_wrap_ui(dashboard_main.ui)

add_executable(dashboard dashboard.cpp dashboard_main.cpp)
target_link_libraries(dashboard Qt5::Widgets foxtrotclient)
target_include_directories(dashboard PRIVATE ${CMAKE_CURRENT_BINARY_DIR} )
