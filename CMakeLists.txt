project(foxtrot C CXX)
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


include(cmake/host_specific.cmake)

include_directories(include ${CMAKE_CURRENT_SOURCE_DIR})

option(BUILD_TESTS "build test code" OFF)
option(BUILD_SERVER "build foxtrot server" ON)
option(BUILD_SETUPS "build experimental setups" ON)
option(BUILD_PROGS "build utility programs" ON)
option(BUILD_DASHBOARD "build monitoring dashboard program" ON)
option(BUILD_OPMD "build OPMD high level client library" OFF)
option(BUILD_KEYPAD "build nostromo n52 keypad library" OFF)
option(BUILD_TELEMETRY "build telemetry server" ON)
option(BUILD_PYTHON "build python bindings" ON)
option(BUILD_UDEV "build udev bindings" OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
#sets which winapi versions are needed, including e.g. killing
#a boost_log_abi error
	add_definitions(-D_WIN32_WINNT=0x601)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  message(WARNING "conanbuildinfo found, building using conan packages")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  set(FOXTROT_CONAN_BUILD TRUE)
  conan_basic_setup()
  add_definitions(-DFOXTROT_CONAN_BUILD=TRUE)
else()
  message(STATUS "conanbuildinfo not found, proceeding with standard build")
endif()


if(FOXTROT_CONAN_BUILD)
	add_definitions(-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)
endif()

set(boost_cmps "log;system;date_time;program_options;filesystem")
include(cmake/libraries.cmake)


if(BUILD_TESTS)
  enable_testing()
  list(APPEND boost_cmps unit_test_framework)
  
endif(BUILD_TESTS)

macro(foxtrot_test testname)
    add_executable(${testname} ${testname}.cpp)
    add_test(${testname} COMMAND ${testname})
    target_link_libraries(${testname} foxtrot ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
endmacro(foxtrot_test)



include_directories(util/PRT/)


include(GNUInstallDirs)

add_subdirectory(src)

include_directories(devices protocols)
add_subdirectory(protocols)
add_subdirectory(devices)
add_subdirectory(expruns)
add_subdirectory(util)
add_subdirectory(watchdog)

if(BUILD_SERVER)
add_subdirectory(server)
add_subdirectory(client)
endif()

if(BUILD_SETUPS)
add_subdirectory(setups)
endif()

add_library(foxtrot SHARED $<TARGET_OBJECTS:foxtrot_obj> $<TARGET_OBJECTS:foxtrot_proto>)


message(WARNING "rttr target: ${RTTR_TARGET}")
target_link_libraries(foxtrot ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY} ${libusb} ${RTTR_TARGET} ${dl} ${CMAKE_THREAD_LIBS_INIT} ${udev_LIBRARIES})

if(FOXTROT_CONAN_BUILD)
target_link_libraries(foxtrot ${Boost_THREAD_LIBRARY})
endif()

install(TARGETS foxtrot EXPORT foxtrotTargets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/ 
ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

file(GLOB ft_includes include/*.h)
message(STATUS "ft includes: ${ft_includes}")
install(FILES ${ft_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ COMPONENT devel)


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
#add_subdirectory(dashboard2)
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

if(BUILD_UDEV)
  add_subdirectory(udev)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/foxtrotConfig.cmake.in foxtrotConfig.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrotConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/ COMPONENT devel)


write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/foxtrotVersion.cmake VERSION 0.0.1 
COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrotVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/ COMPONENT devel)


install(EXPORT foxtrotTargets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/)

