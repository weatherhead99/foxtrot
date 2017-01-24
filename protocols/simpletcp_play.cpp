#include "simpleTCP.h"

#include <iostream>

using namespace foxtrot::protocols;

int main(int argc, char** argv)
{
  parameterset params{ {"port", 10000u} ,
    {"addr", "10.0.0.1"}
  };
  
  simpleTCP proto(&params);
  
  
  proto.Init(nullptr);
  
  proto.write("boom\r\n");
  
  
  auto dat = proto.read(10);
  
  std::cout << "recvd: " << dat << std::endl;
  
  
};