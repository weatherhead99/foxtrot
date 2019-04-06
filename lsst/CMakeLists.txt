project(foxtrotlsst C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)

find_package(foxtrotClient REQUIRED)

set(autofilld_srcs autofilld/autofilld.cpp autofilld/autofill_fill_logic.cpp
    autofilld/autofill_logger.cpp)
add_executable(autofilld ${autofilld_srcs})
target_link_libraries(autofilld foxtrot::foxtrot_client)
