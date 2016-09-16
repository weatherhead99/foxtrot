#include "cornerstone260.h"
#include "SerialPort.h"
#include "SerialProtocol.h"
#include <memory>
#include <iostream>

#include "TestUtilities.h"


int main(int argc, char** argv)
{
  
  foxtrot::parameterset pset {{"port", "/dev/ttyS0"}};
  
  
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>( &pset) ;
  
  foxtrot::expose_cmd<foxtrot::devices::cornerstone260> monoch (std::static_pointer_cast<foxtrot::SerialProtocol>(sport));
  
  
  
  auto repl = monoch.cmd("INFO?");
  std::cout << "repl: " << repl << std::endl;
  
  std::cout << monoch.cmd("UNITS?") << std::endl;
  
  std::cout << monoch.cmd("SHUTTER?") << std::endl;
  
  
  
}