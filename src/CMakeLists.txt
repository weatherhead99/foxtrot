
set(foxtrot_srcs Device.cpp CommunicationProtocol.cpp Error.cpp ProtocolError.cpp  SerialProtocol.cpp)

add_library(foxtrot_obj OBJECT ${foxtrot_srcs})