find_package(Boost REQUIRED COMPONENTS ${boost_cmps})

if(FOXTROT_CONAN_BUILD)
  #hack for grpc address sorting!!
  find_library(address_sorting NAMES address_sorting PATHS ${CONAN_LIB_DIRS_GRPC})
  message(STATUS "address sorting: ${address_sorting}")
endif()  

find_library(libusb NAMES usb-1.0 libusb-1.0 usb)
message(STATUS "libusb: ${libusb}")
find_path(libusb_include NAMES libusb.h usb.h PATH_SUFFIXES libusb-1.0)
message(STATUS "libusb path: ${libusb_include}" )
include_directories(${libusb_include})
include(FindPkgConfig)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux" AND BUILD_UDEV)
	pkg_search_module(udev REQUIRED libudev)
	message("udev libraries: ${udev_LIBRARIES}")
endif()


#C-ares
find_library(cares cares)
message("cares: ${cares}")


#gsl libraries
find_library(gsl gsl)
message("gsl: ${gsl}")
find_library(gslcblas gslcblas)

#curl (for curl request protocol)
find_package(CURL REQUIRED)
message(STATUS "found libcurl: ${CURL_LIBRARIES}")



find_package(rttr 0.9.5 REQUIRED)
message("rttr version: ${rttr_VERSION}")
if(TARGET RTTR::Core)
  message(STATUS "found RTTR shared build")
  set(RTTR_TARGET RTTR::Core)
elseif(TARGET RTTR::Core_Lib)
  message(STATUS "found RTTR static build")
  set(RTTR_TARGET RTTR::Core_Lib)
else()
  message(ERROR "no RTTR found, can't build foxtrot core")
  message(STATUS "considered files: ${rttr_CONSIDERED_CONFIGS}")
endif()

get_target_property(rttr_include_dir ${RTTR_TARGET} INTERFACE_INCLUDE_DIRECTORIES)
message("rttr_include_dir: ${rttr_include_dir}")



find_package(Threads REQUIRED)


#dependencies only required for server
if(BUILD_SERVER)
  find_package(Protobuf REQUIRED)
  set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)
  message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")
  message(STATUS "Protobuf version: ${Protobuf_VERSION}")
  
  find_package(grpc REQUIRED)
  
  find_package(ZLIB REQUIRED )
  find_package(OpenSSL REQUIRED)
  include_directories(${GRPCPP_INCLUDE_DIR})

	if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		find_library(dl dl)
		message(STATUS "libdl: ${dl}")
	else()
		message(STATUS "windows build, don't need libdl")
		set(dl "")
	endif()
endif()

#ONLY required for detector_wl_sweep at the moment!
find_library(CCfits CCfits)
find_library(cfitsio cfitsio)
find_path(CFITSIO_INCLUDE_DIR fitsio.h PATH_SUFFIXES cfitsio)
if(NOT ${cfitsio} STREQUAL cfitsio-NOTFOUND)
	include_directories(${CFITSIO_INCLUDE_DIR})
endif()
