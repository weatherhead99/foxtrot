project(foxtrot_core C CXX)
cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)
#in case of building static libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/)

#import other cmake functionality we need later
include(GNUInstallDirs)
include(FoxtrotCommonSetup)
include(GitVersion)


set(packname "core")
configure_file(${CMAKE_SOURCE_DIR}/share/version.cpp.in
                ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
configure_file(${CMAKE_SOURCE_DIR}/share/version.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/${packname}_version.h)

message("foxtrot core version: ${VERSION}")

if(WIN32)
#need boost::thread on windows
find_package(Boost REQUIRED COMPONENTS log program_options thread)
else()
find_package(Boost REQUIRED COMPONENTS log program_options)
endif()
find_package(Protobuf REQUIRED )
find_package(rttr 0.9.6 REQUIRED)
find_package(gRPC REQUIRED)

#build foxtrot core library (errors and logging)

set(srcs src/backward.cpp src/DeviceError.cpp src/Logging.cpp
src/ProtocolError.cpp src/ProtocolTimeoutError.cpp src/StubError.cpp src/ContentionError.cpp src/Error.cpp src/ProtocolError.cpp src/ServerError.cpp src/TelemetryError.cpp src/ReflectionError.cpp src/config.cpp src/typeUtil.cpp src/ft_tuple_helper.cc src/Device.cpp src/DeviceHarness.cpp src/ExperimentalSetup.cpp src/CmdDevice.cpp)

add_subdirectory(proto)
add_library(foxtrot_core ${srcs} $<TARGET_OBJECTS:foxtrot_protos> ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)

add_dependencies(foxtrot_core copy_protos)

foxtrot_standard_include_dirs(foxtrot_core)
foxtrot_generate_export_header(${packname} foxtrot_core)
target_include_directories(foxtrot_core PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(WIN32)
target_link_libraries(foxtrot_core PUBLIC Boost::log Boost::thread Boost::program_options protobuf::libprotobuf RTTR::Core)
else()
target_link_libraries(foxtrot_core PUBLIC Boost::log Boost::program_options protobuf::libprotobuf  RTTR::Core gRPC::grpc++ gRPC::grpc)
endif()



set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotCore/)
set(ft_core_sourcedir ${CMAKE_CURRENT_SOURCE_DIR})
set(ft_core_binarydir ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS foxtrot_core 
    EXPORT foxtrot_core
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)
    
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/foxtrot/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/foxtrot/
        COMPONENT devel)

install(EXPORT foxtrot_core
        DESTINATION ${ft_cmakedir}
        COMPONENT devel
        NAMESPACE foxtrot::)

foxtrot_create_package_config(cmake/foxtrotCoreConfig.cmake.in ${ft_cmakedir} 
    "ft_cmakedir;ft_core_sourcedir;ft_core_binarydir")

#install cmake macro files
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/FoxtrotCommonSetup.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake.in
        ${CMAKE_CURRENT_LIST_DIR}/cmake/Findlibusb.cmake
        DESTINATION ${ft_cmakedir})

        
#add to package registry
foxtrot_add_to_package_registry(foxtrot_core foxtrotCore)

add_subdirectory(devprogs)
