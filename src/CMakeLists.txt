

set(foxtrot_srcs Device.cpp CommunicationProtocol.cpp Error.cpp ProtocolError.cpp  SerialProtocol.cpp
ProtocolUtilities.cpp CmdDevice.cpp DeviceError.cpp )

add_library(foxtrot_obj OBJECT ${foxtrot_srcs})
target_include_directories(foxtrot_obj INTERFACE $<TARGET_PROPERTY:RTTR::Core,INTERFACE_INCLUDE_DIRECTORIES>)
