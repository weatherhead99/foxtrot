project(foxtrot)
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)



include_directories(include)



option(BUILD_TESTS "build test code" ON)

set(boost_cmps "log")

if(BUILD_TESTS)
  enable_testing()
  list(APPEND boost_cmps unit_test_framework)
  
endif(BUILD_TESTS)

find_package(Boost REQUIRED COMPONENTS ${boost_cmps})


add_subdirectory(src)

add_subdirectory(protocols)
add_subdirectory(devices)

add_library(foxtrot $<TARGET_OBJECTS:foxtrot_obj> $<TARGET_OBJECTS:foxtrot_proto>)


if(BUILD_TESTS)
  add_subdirectory(test)
  
endif(BUILD_TESTS)	