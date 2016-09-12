
set(proto_srcs simpleTCP.cpp SerialPort.cpp BulkUSB.cpp)


find_library(libusb usb-1.0 REQUIRED)
message(STATUS "libusb: ${libusb}")
find_path(libusb_include libusb.h PATH_SUFFIXES libusb-1.0)
message(STATUS "libusb path: ${libusb_include}" )
include_directories(${libusb_include})



include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(foxtrot_proto OBJECT ${proto_srcs})

add_executable(simpletcp_play simpletcp_play.cpp)
target_link_libraries(simpletcp_play foxtrot)


if(BUILD_TESTS)
  add_subdirectory(test)
  
endif(BUILD_TESTS)