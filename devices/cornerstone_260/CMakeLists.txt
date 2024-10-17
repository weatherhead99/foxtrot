add_library(cornerstone260 OBJECT cornerstone260.cpp)
target_sources(cornerstone260 PUBLIC FILE_SET HEADERS FILES cornerstone260.h)

ft_device_lib_configure(cornerstone260)

#add_device_object_lib(NAME cornerstone260 
#SRCS cornerstone260.cpp INCLUDES cornerstone260.h)


# add_executable(cornerstone_play cornerstone_play.cpp)
# add_executable(setwave setwave.cpp)

# target_link_libraries(cornerstone_play cornerstone260 foxtrot)
# target_link_libraries(setwave foxtrot cornerstone260)
