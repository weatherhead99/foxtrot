project(foxtrot_protocols C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "checking for conan build...")
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  message(WARNING "conanbuildinfo detected, building using conan dependencies")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  set(FOXTROT_PROTOCOLS_CONAN_BUILD TRUE)
  conan_basic_setup()
endif()


find_package(foxtrotServer REQUIRED)
include(FoxtrotCommonSetup)
include(GNUInstallDirs)

set(packname "protocols")



set(srcs "")
set(protos "")
set(ft_proto_includes "")

macro(foxtrot_build_protocol protoname)
    option(BUILD_${protoname} "build ${protoname} protocol" ON)
    
    if(BUILD_${protoname})
        list(APPEND srcs src/${protoname}.cpp)
        list(APPEND protos ${protoname})
    endif()
endmacro()


#BulkUSB setup and check
find_package(libusb REQUIRED)
if(${libusb_FOUND})
    message(STATUS "BulkUSB dependencies found")
    foxtrot_build_protocol(BulkUSB)
endif()


#character device has no dependencies
#largely only useful on unix though
foxtrot_build_protocol(characterdevice)

#simpleTCP only needs sockets 
#TODO: needs fully porting to windows!!
foxtrot_build_protocol(simpleTCP)

message(STATUS "finding dependencies for serial port")
find_package(Boost REQUIRED)
if(Boost_FOUND)
    message(STATUS "found serial port dependencies")
    foxtrot_build_protocol(SerialPort)
endif()

#some protocols only work on linux

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  find_file(sg_include scsi/sg.h)
  if(EXISTS ${sg_include})
    message(STATUS "found scsiserial dependencies")
    foxtrot_build_protocol(scsiserial)
  endif()
endif()
#
#i2c protocol currently broken!
#     find_file(i2c_include i2c-dev.h PATH_SUFFIXES linux)
#     message(STATUS "i2c: ${i2c_include}")
#     if(EXISTS ${i2c_include})
#         message(STATUS "found i2c dependencies")
#         foxtrot_build_protocol(i2c)
#     endif()
# endif()

message("protocols enabled for build: ${protos}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(foxtrot_protocols ${srcs})
foxtrot_standard_include_dirs(foxtrot_protocols)
target_link_libraries(foxtrot_protocols PUBLIC foxtrot::foxtrot_core foxtrot::foxtrot_server)

if("BulkUSB" IN_LIST protos)
    message(STATUS "linking in libusb")
    target_link_libraries(foxtrot_protocols PUBLIC libusb::libusb)
endif()


#TODO: do this more elegantly
target_include_directories(foxtrot_protocols PRIVATE ${ft_proto_includes})

install(TARGETS foxtrot_protocols
        EXPORT foxtrot_protocols
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/foxtrot/)

set(ft_cmakedir ${CMAKE_INSTALL_LIBDIR}/cmake/foxtrotProtocols/)

install(EXPORT foxtrot_protocols
        DESTINATION ${ft_cmakedir}
        NAMESPACE foxtrot::
        COMPONENT devel)

set(ft_protocols_sourcedir ${CMAKE_SOURCE_DIR})
foxtrot_create_package_config(cmake/foxtrotProtocolsConfig.cmake.in
                                ${ft_cmakedir}
                                "ft_cmakedir;protos;ft_protocols_sourcedir")

foxtrot_add_to_package_registry(foxtrot_protocols foxtrotProtocols)



