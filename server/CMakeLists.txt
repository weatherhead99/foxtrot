project(foxtrot_server C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(packname "server")

message(STATUS "checking for conan build...")
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message(WARNING "conanbuildinfo detected, building using
    conan dependencies")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    set(FOXTROT_CORE_CONAN_BUILD TRUE)
    conan_basic_setup()
endif()

find_package(foxtrotCore REQUIRED)

find_package(OpenSSL REQUIRED)
find_package(rttr 0.9.6 REQUIRED)
message("rttr version: ${rttr_VERSION}")

find_package(Boost REQUIRED COMPONENTS program_options filesystem)
find_package(CURL REQUIRED)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    find_library(dl dl)
    message(STATUS "libdl: ${dl}")
else()
    message(STATUS "windows build, don't need libdl")
    set(dl "")
endif()

option(BUILD_AUTH "build support for authentication" ON)
if(BUILD_AUTH)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
    message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
    find_package(sodium REQUIRED)
endif()

include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

message("module dir: ${CMAKE_MODULE_PATH}")
include(FoxtrotCommonSetup)


set(server_srcs src/Device.cpp src/DeviceHarness.cpp src/ServerUtil.cpp
src/ProtocolUtilities.cpp src/curlRequest.cpp src/CommunicationProtocol.cpp
src/CmdDevice.cpp)

set(exptserve_srcs src/ServerImpl.cpp src/ServerDescribeImpl.cpp 
    src/InvokeCapabilityImpl.cpp  src/FetchDataImpl.cpp 
    src/ExperimentalSetup.cpp src/ChunkStreamImpl.cpp 
    src/ServerFlagsImpl.cpp src/ServerCommandImpl.cpp src/BroadcastNotificationImpl.cpp src/FetchDataImpl.cpp src/exptserve.cpp src/exptserve_funcs.cpp
    src/pushbullet_api.cpp
    )


if(BUILD_AUTH)
    list(APPEND server_srcs src/AuthHandler.cpp)
    list(APPEND exptserve_srcs src/AuthRequestImpl.cpp)

endif()

add_library(foxtrot_server ${server_srcs})
foxtrot_generate_export_header(${packname} foxtrot_server)
foxtrot_standard_include_dirs(foxtrot_server)
target_include_directories(foxtrot_server PUBLIC ${CURL_INCLUDE_DIR})
target_link_libraries(foxtrot_server PUBLIC foxtrot::foxtrot_core ${CURL_LIBRARIES} RTTR::Core OpenSSL::SSL)

if(BUILD_AUTH)
    target_link_libraries(foxtrot_server PUBLIC ${sodium_LIBRARY_RELEASE})
endif()

add_executable(exptserve ${exptserve_srcs})
target_include_directories(exptserve PRIVATE include/exptserve/)
foxtrot_standard_include_dirs(exptserve)

if(${rttr_VERSION} STREQUAL "0.9.6")
    message(WARNING "new RTTR api used!")
    target_compile_definitions(exptserve PRIVATE -DNEW_RTTR_API)
    target_compile_definitions(foxtrot_server PUBLIC -DNEW_RTTR_API)
endif()

target_link_libraries(exptserve foxtrot::foxtrot_core ${dl} RTTR::Core)
target_link_libraries(exptserve foxtrot_server Boost::filesystem Boost::program_options)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotServer/)

install(TARGETS foxtrot_server
        EXPORT foxtrot_server
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

install(TARGETS exptserve
        EXPORT foxtrot_server
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


install(EXPORT foxtrot_server
        DESTINATION ${ft_cmakedir}
        COMPONENT devel
        NAMESPACE foxtrot::)

foxtrot_create_package_config(cmake/foxtrotServerConfig.cmake.in
                                ${ft_cmakedir}
                                ft_cmakedir)

foxtrot_add_to_package_registry(foxtrot_server foxtrotServer)

add_subdirectory(devprogs)

#build the dummy device
add_subdirectory(devices)
add_subdirectory(setups)
