project(foxtrot_core C CXX)
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
#in case of building static libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/)

#import other cmake functionality we need later
include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FoxtrotCommonSetup)

#fix GRPC windows building issues
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()


option(BUILD_UDEV_SUPPORT "Build UDEV support library (requires libudev)" ON)



#setup conan build if necessary
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
find_package(Protobuf REQUIRED)
find_package(rttr 0.9.6 REQUIRED)
find_package(gRPC REQUIRED)

#configure udev
find_package(libudev)

#build foxtrot core library (errors and logging)
file(GLOB srcs CONFIGURE_DEPENDS src/*.cpp src/*.cc)
add_subdirectory(proto)


#configure building of udev support
if(${BUILD_UDEV_SUPPORT} AND NOT TARGET libudev::libudev)
  message(WARNING "udev support requested but could not find libudev so disabled it")

elseif(${BUILD_UDEV_SUPPORT})
  file(GLOB udev_srcs CONFIGURE_DEPENDS src/udev/*.cpp src/udev/*.cc)
  list(APPEND srcs ${udev_srcs})
endif()

add_library(foxtrot_core ${srcs} ${CMAKE_CURRENT_BINARY_DIR}/version.cpp)
target_compile_definitions(foxtrot_core PUBLIC -DBOOST_LOG_DYN_LINK)


add_dependencies(foxtrot_core copy_protos)

foxtrot_standard_include_dirs(foxtrot_core)
foxtrot_generate_export_header(${packname} foxtrot_core)
target_include_directories(foxtrot_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)


target_link_libraries(foxtrot_core PUBLIC foxtrot_protos Boost::log Boost::program_options
  Boost::filesystem protobuf::libprotobuf
  RTTR::Core)

if(TARGET libudev::libudev)
  message(STATUS "adding libudev to foxtrot_core library dependencies")
  target_link_libraries(foxtrot_core PUBLIC libudev::libudev)

endif()
add_subdirectory(devprogs)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotCore/)
set(ft_core_sourcedir ${CMAKE_CURRENT_SOURCE_DIR})
set(ft_core_binarydir ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS foxtrot_core foxtrot_protos
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

set(ft_core_deps_script
"include(CMakeFindDependencyMacro)
 find_dependency(Boost REQUIRED COMPONENTS system log program_options filesystem thread date_time)
find_dependency(Protobuf REQUIRED)
find_dependency(gRPC REQUIRED)
find_dependency(rttr 0.9.6 REQUIRED)

#WARNING: hack to make it work on windows
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()
"
)

if(TARGET libudev::libudev)
  string(APPEND ft_core_deps_script
    "
   find_dependency(libudev REQUIRED)
")

endif()

foxtrot_setup_cmake_package(foxtrotCore foxtrot_core core ${ft_core_deps_script})

#install cmake macro files
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/FoxtrotCommonSetup.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake
        ${CMAKE_CURRENT_LIST_DIR}/cmake/GetGitRevisionDescription.cmake.in
        ${CMAKE_CURRENT_LIST_DIR}/cmake/Findlibusb.cmake
	${CMAKE_CURRENT_LIST_DIR}/cmake/Findlibudev.cmake
        DESTINATION ${ft_cmakedir})

install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/foxtrotGenericConfig.cmake.in
	DESTINATION ${ft_cmakedir}
	COMPONENT devel)


