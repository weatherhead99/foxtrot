project(foxtrot_protocols C CXX)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


find_package(foxtrotCore REQUIRED)

include(FoxtrotCommonSetup)
include(GNUInstallDirs)

set(packname "protocols")

set(srcs "src/ProtocolUtilities.cpp;src/CommunicationProtocol.cpp")
set(protos "")

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


#this is required on all platforms, otherwise we can't build server
#may change once Direct is useful
find_package(CURL REQUIRED MODULE)
foxtrot_build_protocol(curlRequest)


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

#NOTE: conan fix for system installed rapidjson package
#find_package(RapidJSON REQUIRED MODULE)


add_library(foxtrot_protocols ${srcs})
target_include_directories(foxtrot_protocols PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_link_libraries(foxtrot_protocols PUBLIC foxtrot::foxtrot_core CURL::libcurl)





if("BulkUSB" IN_LIST protos)
    message(STATUS "linking in libusb")
    target_link_libraries(foxtrot_protocols PUBLIC libusb::libusb)
endif()

install(DIRECTORY include/foxtrot
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT devel)


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


foxtrot_setup_cmake_package(cmake/foxtrotProtocolsConfig.cmake.in foxtrotProtocols foxtrot_protocols)



