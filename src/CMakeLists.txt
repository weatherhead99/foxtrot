

set(foxtrot_srcs Device.cpp CommunicationProtocol.cpp Error.cpp ProtocolError.cpp  ProtocolTimeoutError.cpp
SerialProtocol.cpp ProtocolUtilities.cpp CmdDevice.cpp DeviceError.cpp Logging.cpp ContentionError.cpp StubError.cpp)

add_library(foxtrot_obj OBJECT ${foxtrot_srcs})

target_include_directories(foxtrot_obj PUBLIC ${rttr_include_dir})


target_include_directories(foxtrot_obj PRIVATE ${CMAKE_BINARY_DIR}/server/)
target_include_directories(foxtrot_obj PUBLIC ${CMAKE_BINARY_DIR})
target_compile_definitions(foxtrot_obj PRIVATE -DFOXTROT_EXPORTS)

add_executable(logdev logdev.cpp)
target_link_libraries(logdev foxtrot)
