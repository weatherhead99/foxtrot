


find_package(ROOT REQUIRED)

find_library(CCfits CCfits)
find_library(cfitsio cfitsio)

include_directories(${plplot_path}/plplot)

add_executable(detector_wl_sweep detector_wl_sweep.cpp OPMD_setup.cpp wl_sweep_io.cpp)
target_link_libraries(detector_wl_sweep foxtrot cornerstone260 newport2936 Q250  
${plplot} ${ROOT_LIBRARIES} ${gsl} ${gslcblas} ${CCfits} ${cfitsio})

add_executable(simple_pressure_temp_log simple_pressure_temp_log.cpp)
target_link_libraries(simple_pressure_temp_log ${Boost_LIBRARIES} foxtrot TPG362 DM3068 foxtrot_util gsl gslcblas)
