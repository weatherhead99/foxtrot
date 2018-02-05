#hack to build with current bincrafters boost
if(FOXTROT_CONAN_BUILD)
  set(Boost_PROGRAM_OPTIONS_LIBRARY ${CONAN_LIBS_BOOST_PROGRAM_OPTIONS})
  set(Boost_LOG_LIBRARY ${CONAN_LIBS_BOOST_LOG})
  set(Boost_SYSTEM_LIBRARY ${CONAN_LIBS_BOOST_SYSTEM})
  set(Boost_UNIT_TEST_FRAMEWORK_LIBRARY ${CONAN_LIBS_BOOST_TEST})
  set(Boost_THREAD_LIBRARY ${CONAN_LIBS_BOOST_THREAD})
  set(Boost_DATE_TIME_LIBRARY ${CONAN_LIBS_BOOST_DATE_TIME})
else()
  find_package(Boost REQUIRED COMPONENTS ${boost_cmps})
endif()
  

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


  find_library(dl dl)
  message(STATUS "libdl: ${dl}")
  
endif()

#ONLY required for detector_wl_sweep at the moment!
find_library(CCfits CCfits)
find_library(cfitsio cfitsio)
find_path(CFITSIO_INCLUDE_DIR fitsio.h PATH_SUFFIXES cfitsio)
include_directories(${CFITSIO_INCLUDE_DIR})
