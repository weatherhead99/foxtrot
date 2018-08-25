


# find_package(ROOT REQUIRED)

# include_directories(${plplot_path}/plplot)


if(${CCfits} STREQUAL CCfits-NOTFOUND)
	message(WARNING "no CCfits, not building detector_wl_sweep")
else()
	add_executable(detector_wl_sweep detector_wl_sweep.cpp OPMD_setup.cpp wl_sweep_io.cpp)
	target_link_libraries(detector_wl_sweep foxtrot cornerstone260 newport2936R Q250  
	${gsl} ${gslcblas} ${CCfits} ${cfitsio})
endif()
 
 
add_executable(simple_pressure_temp_log simple_pressure_temp_log.cpp)
target_link_libraries(simple_pressure_temp_log ${Boost_DATE_TIME_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} foxtrotclient )

if(${gsl} STREQUAL gsl-NOTFOUND)
	message(WARNING "no GSL, can't build bakeout_tempread")
else()
	add_executable(bakeout_tempread bakeout_tempread.cpp)
	target_link_libraries(bakeout_tempread  foxtrot DM3068  TPG362 foxtrot_util ${gsl} ${gslcblas} ${Boost_DATE_TIME_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})
endif()
