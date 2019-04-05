project(foxtrot_protocols C CXX)
cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(foxtrotCore REQUIRED)
find_package(foxtrotServer REQUIRED)
include(FoxtrotCommonSetup)
include(GNUInstallDirs)

set(packname "protocols")

set(srcs "")
set(protos "")

macro(foxtrot_build_protocol protoname)
    option(BUILD_${protoname} "build ${protoname} protocol" ON)
    
    if(BUILD_${protoname})
        list(APPEND srcs src/${protoname}.cpp)
        list(APPEND protos ${protoname})
    endif()
endmacro()


#BulkUSB setup and check
message(STATUS "finding dependencies for BulkUSB")
find_library(libusb NAMES usb-1.0 libusb-1.0 usb)
find_path(libusb_include NAMES libusb.h usb.h PATH_SUFFIXES libusb-1.0)
if(libusb)
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
if(Boost)
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
    
    find_file(i2c_include i2c-dev.h PATH_SUFFIXES linux)
    message(STATUS "i2c: ${i2c_include}")
    if(EXISTS ${i2c_include})
        message(STATUS "found i2c dependencies")
        foxtrot_build_protocol(i2c)
    endif()
endif()




add_library(foxtrot_protocols ${srcs})
foxtrot_standard_include_dirs(foxtrot_protocols)
target_link_libraries(foxtrot_protocols foxtrot::foxtrot_core foxtrot::foxtrot_server)

#TODO: do this more elegantly
target_include_directories(foxtrot_protocols PRIVATE ${libusb_include})


