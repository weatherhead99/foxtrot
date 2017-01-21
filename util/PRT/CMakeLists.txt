add_library(PRT OBJECT PRT.cpp)
add_executable(PRT_play PRT_play.cpp)
target_link_libraries(PRT_play foxtrot_util gsl gslcblas)