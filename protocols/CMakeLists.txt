
set(proto_srcs simpleTCP.cpp SerialPort.cpp BulkUSB.cpp scsiserial.cpp characterdevice.cpp
i2c.cpp)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(foxtrot_proto OBJECT ${proto_srcs})
target_include_directories(foxtrot_proto INTERFACE $<TARGET_PROPERTY:RTTR::Core,INTERFACE_INCLUDE_DIRECTORIES>)

add_executable(simpletcp_play simpletcp_play.cpp)
target_link_libraries(simpletcp_play foxtrot)

add_executable(serialport_play serialport_play.cpp)
target_link_libraries(serialport_play foxtrot)

add_executable(chardevice_play chardevice_play.cpp)
target_link_libraries(chardevice_play foxtrot foxtrot_util gsl gslcblas)


if(BUILD_TESTS)
  add_subdirectory(test)
  
endif(BUILD_TESTS)
