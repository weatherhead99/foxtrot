add_library(archon archon.cpp archon_modules.cpp archon_module_heaterx.cpp)


add_executable(archon_test archon_test.cpp)
target_link_libraries(archon_test archon foxtrot)
