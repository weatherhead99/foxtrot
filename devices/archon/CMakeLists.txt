set(archon_srcs archon.cpp archon_modules.cpp archon_module_heaterx.cpp archon_autoupdate.cpp archon_module_generic_bias.cpp archon_module_lvxbias.cpp archon_module_hvxbias.cpp archon_module_AD.cpp archon_module_driver.cpp archon_module_xvbias.cpp archon_GPIO.cpp
archonraw.cpp
)

add_device_object_lib(archon ${archon_srcs} "")

# target_link_libraries(archon PRIVATE ${RTTR_TARGET})
# target_link_libraries(archon PUBLIC ${Boost_DATE_TIME_LIBRARY})
# target_compile_definitions(archon PRIVATE -DNEW_RTTR_API)


# add_executable(archon_test archon_test.cpp)
# target_link_libraries(archon_test  ${RTTR_TARGET} foxtrot archon )
