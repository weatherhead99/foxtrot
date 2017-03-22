cmake_host_system_information(RESULT host QUERY HOSTNAME)
message(STATUS "building on host: ${host}")

if( ${host} STREQUAL "pplxdt43")
  set(RTTR_DIR "/home/weatherill/Software/rttr-0.9.5-src/build/install")
  list(APPEND CMAKE_PREFIX_PATH ${RTTR_DIR})
  list(APPEND CMAKE_PREFIX_PATH "/local/home/weatherill/")
#   list(APPEND CMAKE_PREFIX_PATH "/local/home/weatherill/anaconda/")
  set(CMAKE_LIBRARY_ARCHITECTURE x86_64-linux-gnu)
  set(Qt5_DIR "/usr/lib/")
  
#   set(ENV{LD_LIBRARY_PATH} "/local/home/weatherill/lib:/local/home/weatherill/anaconda/lib")
#   set(ENV{LD_RUN_PATH} "/local/home/weatherill/anaconda/lib")
  
#   find_library(png16 png16)
#   message(STATUS "libpng: ${png16}")
#   
#   find_library(icui18n icui18n)
#   message(STATUS "libicui18n: ${icui18n}")
#   
#   find_library(icuuc icuuc)
#   message(STATUS "icuuc: ${icuuc}")
#   
#   find_library(icudata icudata)
#   message(STATUS "icudata: ${icudata}")
# 
#   set(HOST_SPEC_LIBRARIES "${png16};${icui18n};${icuuc};${icudata}")
#   
  
elseif( ${host} STREQUAL "linux-desktop.lan")
    set(RTTR_DIR "/home/danw/Software/rttr-0.9.5-src/build/install")
    list(APPEND CMAKE_PREFIX_PATH ${RTTR_DIR})

    
elseif( ${host} STREQUAL "pplxdaq13")
  set(Qt5_DIR "/usr/lib/x86_64-linux-gnu")
  set(QWT_INCLUDE_DIR "/usr/local/qwt-6.1.3/include")
  set(QWT_LIBRARY "/usr/local/qwt-6.1.3/lib/libqwt.so")
  
    
endif()

