cmake_minimum_required(VERSION 3.25)
project(foxtrot_server C CXX)


option(USE_CORO_IMPLEMENTATIONS "build the c++ coroutine implementation handlers" OFF)
option(BUILD_FTAUTH "build the (deprecated) custom ftauth modules" OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


if(USE_CORO_IMPLEMENTATIONS)
  set(CMAKE_CXX_STANDARD 20)
else()
  set(CMAKE_CXX_STANDARD 17)
endif()


set(CONAN_CMAKE_SILENT_OUTPUT ON)

set(packname "server")


find_package(rttr 0.9.6 REQUIRED)
message("rttr version: ${rttr_VERSION}")
find_package(foxtrotCore REQUIRED)
find_package(foxtrotProtocols REQUIRED)



function(coro_logic target)
  if(${USE_CORO_IMPLEMENTATIONS})
    message(STATUS "attempting using c++20 coroutine implementations")
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
      if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 10)
	message(FATAL_ERROR "coro implementations require gcc 10 or greater!")
      elseif(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 11)
	target_compile_options(${target} PRIVATE -fcoroutines)
      endif()
    endif()
  endif()
endfunction()



#NOTE: the below is to get around gRPC "infinite loop" bugs in their initialization
#if we are using asio-grpc directly from upstream this should work
if(${USE_CORO_IMPLEMENTATIONS})
  
  set(ASIO_GRPC_DISABLE_AUTOLINK TRUE)
  find_package(Boost REQUIRED COMPONENTS program_options system filesystem coroutine)
  find_package(asio-grpc)
else()
  find_package(Boost REQUIRED COMPONENTS program_options system filesystem)
endif()

#however, if we're using the conan package, then the above is currently not supported.
#we can do the below ugly hack to mess around with the deps in that case
if(TARGET asio-grpc_DEPS_TARGET)
  message(WARNING "CONAN BUILD hack: patching asio-grpc deps...")
  set_target_properties(asio-grpc_DEPS_TARGET PROPERTIES INTERFACE_LINK_LIBRARIES
  "$<$<CONFIG:Release>:>;$<$<CONFIG:Release>:>;$<$<CONFIG:Release>:Boost::headers>")
endif()


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)


find_package(RapidJSON REQUIRED)
#on some systems, tehre is a RapidJSON cmake file, but it doesn't export targets, just
#variables
if(${RapidJSON_FOUND} AND NOT TARGET RapidJSON::RapidJSON)
  message(STATUS "manually adding rapidjson target since config file did not...")
  add_library(RapidJSON::RapidJSON INTERFACE IMPORTED GLOBAL)
  set_target_properties(RapidJSON::RapidJSON PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${RAPIDJSON_INCLUDE_DIRS})
endif()


#NOTE: for now, even if FTAUTH isn't built, we still need libsodium for the
# session management API which uses pub/priv keypair
find_package(libsodium REQUIRED)



if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    find_library(dl dl)
    message(STATUS "libdl: ${dl}")
else()
    message(STATUS "windows build, don't need libdl")
    set(dl "")
endif()


include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

message("module dir: ${CMAKE_MODULE_PATH}")
include(FoxtrotCommonSetup)


add_subdirectory(src)


set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotServer/)



install(TARGETS exptserve
       EXPORT foxtrot_server
       RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


install(TARGETS foxtrot_server
       EXPORT foxtrot_server
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
       ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)


install(EXPORT foxtrot_server
       DESTINATION ${ft_cmakedir}
       COMPONENT devel
       NAMESPACE foxtrot::)

install(DIRECTORY include/foxtrot
       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
       COMPONENT devel)
     

install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/Findlibsodium.cmake DESTINATION ${ft_cmakedir})

set(ft_server_dep_str
"
find_dependency(foxtrotCore REQUIRED)
find_dependency(Boost REQUIRED COMPONENTS program_options filesystem asio)
find_dependency(CURL REQUIRED)
find_dependency(RapidJSON REQUIRED MODULE)
find_dependency(libsodium REQUIRED)
find_dependency(rttr 0.9.6 REQUIRED)

set(ASIO_GRPC_DISABLE_AUTOLINK TRUE)
find_dependency(asio-grpc)

#however, if we're using the conan package, then the above is currently not supported.
#we can do the below ugly hack to mess around with the deps in that case
if(TARGET asio-grpc_DEPS_TARGET)
  message(WARNING 'CONAN BUILD hack: patching asio-grpc deps...')
  set_target_properties(asio-grpc_DEPS_TARGET PROPERTIES INTERFACE_LINK_LIBRARIES
  '$<$<CONFIG:Release>:>;$<$<CONFIG:Release>:>;$<$<CONFIG:Release>:Boost::headers>')

endif()


if(${CMAKE_SYSTEM_NAME} STREQUAL 'Linux')
    find_library(dl dl)
    message(STATUS 'libdl: ${dl}')
else()
    message(STATUS 'windows build, don't need libdl')
    set(dl '')
endif()
")

foxtrot_standard_setup_cmake_package(server ${ft_server_dep_str})


add_subdirectory(devprogs)

#build the dummy device
add_subdirectory(devices)
add_subdirectory(setups)
