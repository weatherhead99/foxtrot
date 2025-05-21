add_library(Pfeiffer OBJECT TPG362.cpp Pfeiffer.cpp TC110.cpp)
file(GLOB pfeiffer_includes *.h)
target_sources(Pfeiffer PUBLIC FILE_SET HEADERS FILES ${pfeiffer_includes})
ft_device_lib_configure(Pfeiffer)

add_executable(tpgnetplay TPG366_network_play.cpp)
target_link_libraries(tpgnetplay foxtrot::foxtrot_core Pfeiffer)

		      

# add_executable(checksumtest test_checksum.cpp)
# add_executable(tpgtest TPG362_play.cpp)
# 
# target_link_libraries(tpgtest foxtrot TPG362 )
