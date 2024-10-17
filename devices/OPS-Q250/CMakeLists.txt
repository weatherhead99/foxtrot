add_library(Q250 OBJECT Q250.cpp)
target_sources(Q250 PUBLIC FILE_SET HEADERS FILES Q250.h)
ft_device_lib_configure(Q250)

# add_executable(Q250_play Q250_play.cpp)
# target_link_libraries(Q250_play Q250 foxtrot)
