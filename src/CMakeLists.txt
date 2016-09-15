
set(foxtrot_srcs Device.cpp CommunicationProtocol.cpp Error.cpp ProtocolError.cpp  SerialProtocol.cpp
ProtocolUtilities.cpp CmdDevice.cpp)

add_library(foxtrot_obj OBJECT ${foxtrot_srcs})