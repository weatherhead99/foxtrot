#include "newport2936R.h"
#include "BulkUSB.h"
#include "SerialPort.h"
#include <iostream>
#include "DeviceError.h"
#include <algorithm>
#include <string>
#include <rttr/registration>

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

  
foxtrot::devices::powerunits foxtrot::devices::newport2936R::getUnits()
{
  auto repl = cmd("PM:UNIT?");
  
  return static_cast<foxtrot::devices::powerunits>(std::stoi(repl));

}

void foxtrot::devices::newport2936R::setUnits(foxtrot::devices::powerunits unit)
{
  _proto->write(std::string("PM:UNIT ") + std::to_string(static_cast<unsigned>(unit))+'\r');
  
}


const std::string foxtrot::devices::newport2936R::getDeviceTypeName() const
{
  return "newport2936R";
}

std::string convert_powerunit_to_string(foxtrot::devices::powerunits unit, bool&ok)
{
  using foxtrot::devices::powerunits;
  ok = true;
  switch(unit)
  {
    case(powerunits::Amps): return "A";
    case(powerunits::Volts): return "V";
    case(powerunits::Watts): return "W";
    case(powerunits::Watts_cm2): return "W/cm2";
    case(powerunits::Joules): return "J";
    case(powerunits::Joules_cm2): return "J/cm2";
      
  }
  
}


foxtrot::devices::powerunits convert_string_to_powerunit(const std::string& s, bool& ok)
{
  using foxtrot::devices::powerunits;
  ok = true;
  
  if(s == "A")
  {
    return powerunits::Amps;
  }
  else if (s == "V")
  {
    return powerunits::Volts;
  }
  else if(s == "W")
  {
    return powerunits::Watts;
  }
  else if(s == "W/cm2")
  {
    return powerunits::Watts_cm2;
  }
  else if(s == "J")
  {
    return powerunits::Joules;
  }
  else if(s == "J/cm2")
  {
    return powerunits::Joules_cm2;
  }
  else
  {
    ok = false;
    return powerunits::Amps;
  }
  
}


RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::newport2936R;
  
  registration::class_<newport2936R>("foxtrot::devices::newport2936R")
  .method("setLambda",&newport2936R::setLambda)
  (
    parameter_names("l")
    )
  .property_readonly("getLambda",&newport2936R::getLambda)
  .property_readonly("getPower",&newport2936R::getPower)
  .property_readonly("getResponsivity", &newport2936R::getResponsivity)
  .property_readonly("getArea",&newport2936R::getArea)
  .property_readonly("getUnits",&newport2936R::getUnits)
  .method("setUnits",&newport2936R::setUnits)
  (
    parameter_names("unit")
    )
  ;
  
  rttr::type::register_converter_func(convert_powerunit_to_string);
  rttr::type::register_converter_func(convert_string_to_powerunit);
  
}
