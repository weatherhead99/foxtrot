project(foxtrot)
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)



include_directories(include ${CMAKE_CURRENT_SOURCE_DIR})



option(BUILD_TESTS "build test code" ON)

set(boost_cmps "log;system")

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


find_library(libusb usb-1.0 REQUIRED)
message(STATUS "libusb: ${libusb}")
find_path(libusb_include libusb.h PATH_SUFFIXES libusb-1.0)
message(STATUS "libusb path: ${libusb_include}" )
include_directories(${libusb_include})

add_subdirectory(src)

include_directories(devices protocols)
add_subdirectory(protocols)
add_subdirectory(devices)


add_library(foxtrot $<TARGET_OBJECTS:foxtrot_obj> $<TARGET_OBJECTS:foxtrot_proto>)
target_link_libraries(foxtrot ${Boost_SYSTEM_LIBRARY} ${libusb})

if(BUILD_TESTS)
  
  add_subdirectory(test)
  
endif(BUILD_TESTS)	