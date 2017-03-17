cmake_host_system_information(RESULT host QUERY HOSTNAME)
message(STATUS "building on host: ${host}")

if( ${host} STREQUAL "pplxdt43")
  set(RTTR_DIR "/home/weatherill/Software/rttr-0.9.5-src/build/install")
  list(APPEND CMAKE_PREFIX_PATH ${RTTR_DIR})
  list(APPEND CMAKE_PREFIX_PATH "/local/home/weatherill/")
  set(CMAKE_LIBRARY_ARCHITECTURE x86_64-linux-gnu)
  set(ENV{LD_LIBRARY_PATH} "/local/home/weatherill/lib:/local/home/weatherill/anaconda/lib")
  


elseif( ${host} STREQUAL "linux-desktop.lan")
    set(RTTR_DIR "/home/danw/Software/rttr-0.9.5-src/build/install")
    list(APPEND CMAKE_PREFIX_PATH ${RTTR_DIR})

    
elseif( ${host} STREQUAL "pplxdaq13")
  set(Qt5_DIR "/usr/lib/x86_64-linux-gnu")
  set(QWT_INCLUDE_DIR "/usr/local/qwt-6.1.3/include")
  set(QWT_LIBRARY "/usr/local/qwt-6.1.3/lib/libqwt.so")
  
    
endif()

