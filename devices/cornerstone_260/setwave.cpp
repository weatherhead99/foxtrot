#include <iostream>

#include <string>
#include <memory>

#include "cornerstone_260/cornerstone260.h"
#include "SerialPort.h"
#include "TestUtilities.h"

int main(int argc, char** argv)
{
  
  std::string inp(argv[1]);
  auto num = std::stod(inp);
  
  std::cout << "setting wave to: " << num << " nm" << std::endl;
  
  foxtrot::parameterset pset {{"port", "/dev/ttyS0"}};
  
  
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&pset);
  
  foxtrot::expose_cmd<foxtrot::devices::cornerstone260> monoch (std::static_pointer_cast<foxtrot::SerialProtocol>(sport));
  
  
  
  monoch.setWave(num);
  
  
  
  
}