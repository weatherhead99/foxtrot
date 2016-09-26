#include "newport2936R.h"
#include "BulkUSB.h"
#include "SerialPort.h"
#include <iostream>
#include "DeviceError.h"
#include <algorithm>


const foxtrot::parameterset newport2936R_usb_params 
{
  {"vid", 0x104du},
  {"pid", 0xcec7u},
  {"epin", 0x81u},
  {"epout", 0x02u}
  
};

const foxtrot::parameterset newport2936R_serial_params
{
  {"baudrate", 38400u},
  {"stopbits",  1},
  {"bits",  8u},
  {"parity",  "none"},
  {"flowcontrol", "none"}
    
  };
  



foxtrot::devices::newport2936R::newport2936R(std::shared_ptr< foxtrot::SerialProtocol> proto)
: CmdDevice(proto), _proto(proto)
{
  
  
  auto specproto = std::dynamic_pointer_cast<foxtrot::protocols::BulkUSB>(proto);
  if(specproto != nullptr)
  {
    std::cout << "using usb connected power meter" << std::endl;
    specproto->Init(&newport2936R_usb_params);
    _usbmode = true;
    
    
  }
  else if(  auto serproto = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(proto)   != nullptr)
  {
    std::cout << "using serial connected power meter" << std::endl;
    specproto->Init(&newport2936R_serial_params);
    _usbmode = false;
    
  }
  
  

}

std::string foxtrot::devices::newport2936R::cmd(const std::string& request)
{
  
  _proto->write(request + '\r');
  
  
  unsigned actlen;
  auto buffer = _proto->read(64, &actlen);
  
  //TODO: off by one error here?
  
  
  return std::string(buffer.begin(), buffer.begin() + actlen);
  
  
}


int foxtrot::devices::newport2936R::getLambda()
{
  auto lambdastr = cmd("PM:L?");
  strip_CRLF(lambdastr);
  
  return std::stoi(lambdastr);

}



void foxtrot::devices::newport2936R::setLambda(int l)
{
  _proto->write(std::string("PM:L ") + std::to_string(l) +'\r');
  
}


double foxtrot::devices::newport2936R::getPower()
{
  auto repl = cmd("PM:P?");
  strip_CRLF(repl);
  
  return std::stod(repl);
}



double foxtrot::devices::newport2936R::getResponsivity()
{
  auto repl = cmd("PM:Responsivity?");
  strip_CRLF(repl);
  return std::stod(repl);

}

double foxtrot::devices::newport2936R::getArea()
{
  auto repl = cmd("PM:DETSIZE?");
  strip_CRLF(repl);
  return std::stod(repl);

}


void foxtrot::devices::newport2936R::strip_CRLF(std::string& buffer)
{
  auto crpos = std::find(buffer.begin(),buffer.end(),'\r');
  buffer = std::string(buffer.begin(),crpos);
  
}

  

