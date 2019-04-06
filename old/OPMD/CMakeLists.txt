add_library(OPMD OPMD_hlclient.cpp)
target_link_libraries(OPMD foxtrotclient)
target_include_directories(OPMD INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})