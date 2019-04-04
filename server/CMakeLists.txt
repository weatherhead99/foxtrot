project(foxtrot_server C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(packname "server")

find_package(foxtrotCore REQUIRED)

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


include(GenerateExportHeader)
include(GNUInstallDirs)


message("module dir: ${CMAKE_MODULE_PATH}")
include(FoxtrotCommonSetup)


set(server_srcs src/Device.cpp src/DeviceHarness.cpp src/ServerUtil.cpp
src/ProtocolUtilities.cpp src/curlRequest.cpp src/CommunicationProtocol.cpp)

set(exptserve_srcs src/ServerImpl.cpp src/ServerDescribeImpl.cpp 
    src/InvokeCapabilityImpl.cpp  src/FetchDataImpl.cpp 
    src/ExperimentalSetup.cpp src/ChunkStreamImpl.cpp 
    src/ServerFlagsImpl.cpp src/ServerCommandImpl.cpp src/BroadcastNotificationImpl.cpp
    src/config.cpp src/FetchDataImpl.cpp src/exptserve.cpp src/exptserve_funcs.cpp
    src/pushbullet_api.cpp
    )

add_library(foxtrot_server ${server_srcs})
foxtrot_generate_export_header(${packname} foxtrot_server)
target_include_directories(foxtrot_server PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    ${CURL_INCLUDE_DIR})
    
target_include_directories(foxtrot_server PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(foxtrot_server foxtrot::foxtrot_core ${CURL_LIBRARIES})

add_executable(exptserve ${exptserve_srcs})
target_include_directories(exptserve PRIVATE include/exptserve/)
target_include_directories(exptserve PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

if(${rttr_VERSION} STREQUAL "0.9.6")
    message(WARNING "new RTTR api used!")
    target_compile_definitions(exptserve PRIVATE -DNEW_RTTR_API)
    target_compile_definitions(foxtrot_server PUBLIC -DNEW_RTTR_API)
endif()
    
target_include_directories(exptserve PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

    
target_link_libraries(exptserve foxtrot::foxtrot_core ${dl} RTTR::Core)
target_link_libraries(exptserve foxtrot_server Boost::filesystem Boost::program_options)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrot/)

install(TARGETS foxtrot_server
        EXPORT foxtrot_server
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

install(TARGETS exptserve
        EXPORT foxtrot_server
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


install(DIRECTORY include/foxtrot
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT devel)
        
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/foxtrot_server_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/
        COMPONENT devel)

install(EXPORT foxtrot_server
        DESTINATION ${ft_cmakedir}
        COMPONENT DEVEL
        NAMESPACE foxtrot::)
