project(foxtrot C CXX)
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(cmake/host_specific.cmake)

include_directories(include ${CMAKE_CURRENT_SOURCE_DIR})


option(BUILD_TESTS "build test code" ON)
option(BUILD_SERVER "build foxtrot server" ON)
option(BUILD_TELEMETRY "build telemetry server" ON)
option(BUILD_PYTHON "build python bindings" ON)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  message(WARNING "conanbuildinfo found, building using conan packages")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
else()
  message(STATUS "conanbuildinfo not found, proceeding with standard build")
endif()


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

include(GNUInstallDirs)

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


add_library(foxtrot SHARED $<TARGET_OBJECTS:foxtrot_obj> $<TARGET_OBJECTS:foxtrot_proto>)
target_link_libraries(foxtrot ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY} ${libusb} RTTR::Core ${dl} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS foxtrot EXPORT ft_targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/ )

file(GLOB ft_includes include/*.h)
message(STATUS "ft includes: ${ft_includes}")
install(FILES ${ft_includes} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/ COMPONENT devel)


get_target_property(foxtrot_transitive_link foxtrot INTERFACE_LINK_LIBRARIES)
message("foxtrot ILLs: ${foxtrot_transitive_link}")


if(BUILD_TESTS)  
  add_subdirectory(test)
endif(BUILD_TESTS)	


if(BUILD_TELEMETRY)
  add_subdirectory(telemetry)
endif()

include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/foxtrotConfig.cmake.in foxtrotConfig.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrotConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/ COMPONENT devel)


write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/foxtrotVersion.cmake VERSION 0.0.1 
COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrotVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/ COMPONENT devel)


