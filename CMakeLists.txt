project(foxtrot C CXX)
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(cmake/host_specific.cmake)

include_directories(include ${CMAKE_CURRENT_SOURCE_DIR})


option(BUILD_TESTS "build test code" ON)
option(BUILD_SERVER "build foxtrot server" ON)
option(BUILD_SETUPS "build experimental setups" ON)
option(BUILD_PROGS "build utility programs" ON)
option(BUILD_DASHBOARD "build monitoring dashboard program" ON)
option(BUILD_OPMD "build OPMD high level client library" ON)
option(BUILD_KEYPAD "build nostromo n52 keypad library" ON)
option(BUILD_TELEMETRY "build telemetry server" ON)

set(boost_cmps "log;system;date_time;program_options")

if(BUILD_TESTS)
  enable_testing()
  list(APPEND boost_cmps unit_test_framework)
  
endif(BUILD_TESTS)

macro(foxtrot_test testname)
    add_executable(${testname} ${testname}.cpp)
    add_test(${testname} COMMAND ${testname})
    target_link_libraries(${testname} foxtrot ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
endmacro(foxtrot_test)

include(cmake/libraries.cmake)


include_directories(${rttr_include})


include_directories(util/PRT/)


add_subdirectory(src)

include_directories(devices protocols)
add_subdirectory(protocols)
add_subdirectory(devices)
add_subdirectory(expruns)
add_subdirectory(util)

if(BUILD_SERVER)
add_subdirectory(server)
add_subdirectory(client)
endif()

if(BUILD_SETUPS)
add_subdirectory(setups)
endif()

add_library(foxtrot SHARED $<TARGET_OBJECTS:foxtrot_obj> $<TARGET_OBJECTS:foxtrot_proto>)
target_link_libraries(foxtrot ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY} ${libusb} RTTR::Core ${dl})



get_target_property(foxtrot_transitive_link foxtrot INTERFACE_LINK_LIBRARIES)
message("foxtrot ILLs: ${foxtrot_transitive_link}")

if(BUILD_PROGS)
add_subdirectory(tbprogs)
endif()


if(BUILD_DASHBOARD)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
get_target_property(qcore Qt5::Core  LOCATION_Release)
message(STATUS "qt: ${qcore}")
find_package(Qwt REQUIRED)
set(CMAKE_AUTOMOC ON)

add_subdirectory(dashboard)
add_subdirectory(dashboard2)
endif()

if(BUILD_TESTS)  
  add_subdirectory(test)
endif(BUILD_TESTS)	

if(BUILD_OPMD)
  add_subdirectory(OPMD)
endif()

if(BUILD_KEYPAD)
  add_subdirectory(keypad)
endif()

if(BUILD_TELEMETRY)
  add_subdirectory(telemetry)
endif()
