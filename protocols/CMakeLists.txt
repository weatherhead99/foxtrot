cmake_minimum_required(VERSION 3.25)
project(foxtrot_protocols C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(foxtrotCore REQUIRED)
include(FoxtrotCommonSetup)

set(packname "protocols")


option(USE_ASIO "use ASIO implementations rather than polling" ON)


add_library(foxtrot_protocols src/CommunicationProtocol.cpp src/ProtocolUtilities.cpp)
set(ft_protocols_deps_script
  "
find_dependency(foxtrotCore REQUIRED)
find_dependency(CURL REQUIRED)
")

#the basic sources
target_sources(foxtrot_protocols PUBLIC FILE_SET HEADERS FILES
  include/foxtrot/protocols/ProtocolUtilities.h include/foxtrot/protocols/CommunicationProtocol.h
BASE_DIRS include)


find_package(libusb)
if(NOT libusb_FOUND)
  message(WARNING "libusb not found, will not build libusb based protocols!")
else()
  message(NOTICE "found libusb, building libusb based protocols")
  target_sources(foxtrot_protocols PRIVATE src/BulkUSB.cpp src/libUsbProtocol.cpp)
  target_sources(foxtrot_protocols PUBLIC FILE_SET HEADERS FILES
    include/foxtrot/protocols/BulkUSB.h
    include/foxtrot/protocols/libUsbProtocol.hh BASE_DIRS include)
  list(APPEND protos "BulkUSB")
  list(APPEND protoos "libUsbProtocol")
  target_link_libraries(foxtrot_protocols PUBLIC libusb::libusb)

    set(ft_protocols_deps_script "${ft_protocols_deps_script}
     find_dependency(libusb REQUIRED)"
    )
endif()


#this is required on all platforms, otherwise we can't build server
#may change once Direct is useful
find_package(CURL REQUIRED)
target_sources(foxtrot_protocols PRIVATE src/curlRequest.cpp)
target_sources(foxtrot_protocols PUBLIC FILE_SET HEADERS FILES include/foxtrot/protocols/curlRequest.h BASE_DIRS include)
list(APPEND protos "curlRequest")

#add stuff that needs no dependencies
#NOTE serialport depends on Boost, but we must have had that to build foxtrotCore anyway
target_sources(foxtrot_protocols PRIVATE src/characterdevice.cpp src/simpleTCP.cpp src/SerialPort.cpp)
target_sources(foxtrot_protocols PUBLIC FILE_SET HEADERS FILES include/foxtrot/protocols/characterdevice.h
  include/foxtrot/protocols/simpleTCP.h include/foxtrot/protocols/SerialPort.h BASE_DIRS include)


if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  find_file(sg_include scsi/sg.h)
  if(EXISTS ${sg_include})
    message(NOTICE "found scsiserial dependencies")
    target_sources(foxtrot_protocols PRIVATE src/scsiserial.cpp)
    target_sources(foxtrot_protocols PUBLIC FILE_SET HEADERS FILES include/foxtrot/protocols/scsiserial.h BASE_DIRS include)

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


#NOTE: conan fix for system installed rapidjson package
#find_package(RapidJSON REQUIRED MODULE)


#add_library(foxtrot_protocols ${srcs})
target_link_libraries(foxtrot_protocols PUBLIC foxtrot::foxtrot_core CURL::libcurl)

if(USE_ASIO)
  target_compile_definitions(foxtrot_protocols PRIVATE FT_USE_ASIO_IMPLS=1)
endif()

install(TARGETS foxtrot_protocols
  EXPORT foxtrot_protocols
  PUBLIC_HEADER COMPONENT devel FILE_SET HEADERS)


add_subdirectory(devprogs)


foxtrot_standard_setup_cmake_package(protocols ${ft_protocols_deps_script})


