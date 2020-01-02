add_device_object_lib(NAME TPG362 
                        SRCS TPG362.cpp Pfeiffer.cpp TC110.cpp
                        INCLUDES TPG362.h Pfeiffer.h TC110.h)


# add_executable(checksumtest test_checksum.cpp)
# add_executable(tpgtest TPG362_play.cpp)
# 
# target_link_libraries(tpgtest foxtrot TPG362 )
