



set(proto_srcs simpleTCP.cpp SerialPort.cpp BulkUSB.cpp characterdevice.cpp curlRequest.cpp)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	list(APPEND proto_srcs scsiserial.cpp)
endif()

option(BUILD_I2C OFF)
if(BUILD_I2C)
  list(APPEND proto_srcs i2c.cpp)
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(foxtrot_proto OBJECT ${proto_srcs})
target_include_directories(foxtrot_proto INTERFACE $<TARGET_PROPERTY:RTTR::Core,INTERFACE_INCLUDE_DIRECTORIES>)
target_include_directories(foxtrot_proto PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
target_compile_definitions(foxtrot_proto PRIVATE -Dfoxtrot_EXPORTS)


add_executable(simpletcp_play simpletcp_play.cpp)
target_link_libraries(simpletcp_play foxtrot )

add_executable(serialport_play serialport_play.cpp)
target_link_libraries(serialport_play foxtrot)


if(${gsl} STREQUAL gsl-NOTFOUND)
   message(WARNING "no gsl, can't build chardevice_play")
else()
   add_executable(chardevice_play chardevice_play.cpp)
   target_link_libraries(chardevice_play foxtrot foxtrot_util gsl gslcblas)
endif()


if(BUILD_TESTS)
  add_subdirectory(test)
  
endif(BUILD_TESTS)
