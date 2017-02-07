find_package(Boost REQUIRED COMPONENTS ${boost_cmps})

message("Boost libraries ${Boost_LIBRARIES}")

find_library(libusb usb-1.0 REQUIRED)
message(STATUS "libusb: ${libusb}")
find_path(libusb_include libusb.h PATH_SUFFIXES libusb-1.0)
message(STATUS "libusb path: ${libusb_include}" )
include_directories(${libusb_include})


#gsl libraries
find_library(gsl gsl)
message("gsl: ${gsl}")
find_library(gslcblas gslcblas)


find_package(rttr REQUIRED)
get_property(rttr_include TARGET RTTR::Core PROPERTY INTERFACE_INCLUDE_DIRECTORIES)


#dependencies only required for server
if(BUILD_SERVER)
  find_package(Protobuf REQUIRED)
  set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)
  message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
  
  find_package(grpc REQUIRED)
  
  find_package(ZLIB REQUIRED )
  find_package(OpenSSL REQUIRED)
  include_directories(${GRPCPP_INCLUDE_DIR})


  find_library(dl dl)
  message(STATUS "libdl: ${dl}")
  
endif()


find_library(CCfits CCfits)
find_library(cfitsio cfitsio)
find_path(CFITSIO_INCLUDE_DIR fitsio.h PATH_SUFFIXES cfitsio)
include_directories(${CFITSIO_INCLUDE_DIR})
