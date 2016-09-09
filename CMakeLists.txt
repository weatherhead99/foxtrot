project(foxtrot)
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)



include_directories(include ${CMAKE_CURRENT_SOURCE_DIR})



option(BUILD_TESTS "build test code" ON)

set(boost_cmps "log")

if(BUILD_TESTS)
  enable_testing()
  list(APPEND boost_cmps unit_test_framework)
  
endif(BUILD_TESTS)

macro(foxtrot_test testname)
    add_executable(${testname} ${testname}.cpp)
    add_test(${testname} COMMAND ${testname})
    target_link_libraries(${testname} foxtrot ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
endmacro(foxtrot_test)



find_package(Boost REQUIRED COMPONENTS ${boost_cmps})


add_subdirectory(src)

add_subdirectory(protocols)
add_subdirectory(devices)

add_library(foxtrot $<TARGET_OBJECTS:foxtrot_obj> $<TARGET_OBJECTS:foxtrot_proto>)


if(BUILD_TESTS)
  
  add_subdirectory(test)
  
endif(BUILD_TESTS)	