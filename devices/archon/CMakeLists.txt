add_library(archon archon.cpp)


add_executable(archon_test archon_test.cpp)
target_link_libraries(archon_test archon foxtrot)