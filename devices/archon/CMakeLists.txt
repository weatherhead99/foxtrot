add_library(archon archon.cpp archon_modules.cpp archon_module_heaterx.cpp archon_autoupdate.cpp)
target_link_libraries(archon RTTR::Core)

add_executable(archon_test archon_test.cpp)
target_link_libraries(archon_test archon foxtrot ${OPENSSL_LIBRARIES})
