#include "newport2936R.h"
#include "BulkUSB.h"
#include "SerialPort.h"
#include <iostream>
#include "DeviceError.h"


const foxtrot::parameterset newport2936R_usb_params 
{
  {"vid", 0x104d},
  {"pid", 0xcec7},
  {"epin", 0x81},
  {"epout", 0x02}
  
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
    
  }
  else if(  auto serproto = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(proto)   != nullptr)
  {
    std::cout << "using serial connected power meter" << std::endl;
    specproto->Init(&newport2936R_serial_params);
    
  }
  
  

}

std::string foxtrot::devices::newport2936R::cmd(const std::string& request)
{
  
  _proto->write(request + '\r');
  
  std::array<char, 64> buffer;
  
  unsigned actlen;
  _proto->read(64, &actlen);
  
  //TODO: off by one error here?
  return std::string(buffer.begin(), buffer.begin() + actlen);
  
  
}
