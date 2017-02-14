macro(add_device_lib name srcfiles)
add_library(${name} STATIC ${srcfiles})
list(APPEND device_libs ${name})


endmacro()



add_subdirectory(archon)
add_subdirectory(cornerstone_260)
add_subdirectory(OPS-Q250)
add_subdirectory(BSC203)
add_subdirectory(newport_2936R)
add_subdirectory(TPG362)
add_subdirectory(DM3068)
add_subdirectory(dummy)

add_subdirectory(BME280)
