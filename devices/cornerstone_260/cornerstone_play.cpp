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
  
  
  auto info = monoch.cmd("INFO?");
  std::cout << "INFO: " << info << std::endl;
  

  auto shutter = monoch.getShutterStatus();
  std::cout << "SHUTTER:" << shutter << std::endl;

  auto word = shutter ? "closing" : "opening";

  std::cout << word<<"  shutter" << std::endl;
  monoch.setShutterStatus(!shutter);
  std::cout << "SHUTTER:" << monoch.getShutterStatus() << std::endl;


  
  
}
