#setup conan build if necessary
message(STATUS "checking for conan build...")
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message(WARNING "conanbuildinfo detected, building using
    conan dependencies")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(FOXTROT_CORE_CONAN_BUILD TRUE)
    conan_basic_setup()
endif()


#set shared library build by default
set(BUILD_SHARED_LIBS ON CACHE BOOL "build shared libraries")
