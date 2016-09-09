
set(proto_srcs simpleTCP.cpp SerialPort.cpp)

add_library(foxtrot_proto OBJECT ${proto_srcs})

add_executable(simpletcp_play simpletcp_play.cpp)
target_link_libraries(simpletcp_play foxtrot)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(BUILD_TESTS)
  add_subdirectory(test)
  
endif(BUILD_TESTS)