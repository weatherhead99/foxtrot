file(GLOB archon_srcs CONFIGURE_DEPENDS *.cpp)
file(GLOB archon_includes CONFIGURE_DEPENDS *.h)

add_library(archon OBJECT ${archon_srcs})
target_sources(archon PUBLIC FILE_SET HEADERS FILES ${archon_includes})
target_link_libraries(archon PUBLIC Boost::date_time RTTR::Core)
target_compile_definitions(archon PRIVATE -DNEW_RTTR_API)
target_compile_options(archon PRIVATE -Wall)


ft_device_lib_configure(archon)

# add_executable(archon_test archon_test.cpp)
# target_link_libraries(archon_test  ${RTTR_TARGET} foxtrot archon )
