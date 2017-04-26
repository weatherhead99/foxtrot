#include "newport2936R.h"
#include "BulkUSB.h"
#include "SerialPort.h"
#include <iostream>
#include "DeviceError.h"
#include <algorithm>
#include <string>
#include <rttr/registration>
#include <thread>
#include <chrono>
#include <sstream>

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
  {"stopbits",  1u},
  {"bits",  8u},
  {"parity",  "none"},
  {"flowcontrol", "none"}
    
  };
  



foxtrot::devices::newport2936R::newport2936R(std::shared_ptr< foxtrot::SerialProtocol> proto)
: CmdDevice(proto), _proto(proto), _lg("newport2936R")
{
  
  
  auto specproto = std::dynamic_pointer_cast<foxtrot::protocols::BulkUSB>(proto);
  auto serproto = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(proto);
  if(specproto != nullptr)
  {
    _lg.Info( "using usb connected power meter");
    specproto->Init(&newport2936R_usb_params);
    _lg.Debug("init done");
    _usbmode = true;
    
    
  }
  else if(  serproto   != nullptr)
  {
    _lg.Info("using serial connected power meter" );
    serproto->Init(&newport2936R_serial_params);
    _lg.Debug("init done");
    
    _lg.Debug("flushing serial port");
    serproto->flush();
    
    _lg.Debug("disabling command echo");
    serproto->write("ECHO 0\r");
    _usbmode = false;
    
  }
  
  

}

std::string foxtrot::devices::newport2936R::cmd(const std::string& request)
{
  
  _lg.Debug("request is: " + request);
  _proto->write(request + '\r');
  
  if(!_usbmode)
  {
    auto specproto = static_cast<foxtrot::protocols::SerialPort*>(_proto.get());
    specproto->flush();
  }
  
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
  repl.erase(std::remove_if(repl.begin(),repl.end(), ::isspace),repl.end());
  
  
  _lg.Trace("units reply: " + repl);
  _lg.Trace("units reply length: " + std::to_string(repl.size()));
  
  _lg.Trace("stoi: " + std::to_string(std::stoi(repl)));
  
  
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
  foxtrot::Logging lg("convert_powerunit_to_string");
  
  lg.Trace("converting powerunit to string");
  
  using foxtrot::devices::powerunits;
  ok = true;
  switch(unit)
  {
    case(powerunits::Amps): return std::string("A");
    case(powerunits::Volts): return std::string("V");
    case(powerunits::Watts): return std::string("W");
    case(powerunits::Watts_cm2): return std::string("W/cm2");
    case(powerunits::Joules): return std::string("J");
    case(powerunits::Joules_cm2): return std::string("J/cm2");
    default:
      ok = false;
      return std::string("ERROR");
  }
  
}

foxtrot::devices::powermodes foxtrot::devices::newport2936R::getMode()
{
  auto repl = cmd("PM:MODE?");
  
  powermodes  sw = static_cast<powermodes>(std::stoi(repl));
  return sw;
}


foxtrot::devices::powermodes convert_int_to_mode(int s, bool& ok)
{
  if(s > 7 || s < 0)
  {
    ok = false;
  }
  
  ok = true;
  return static_cast<foxtrot::devices::powermodes>(s);
}

int convert_mode_to_int(foxtrot::devices::powermodes mode, bool& ok)
{
  ok = true;
  return static_cast<int>(mode);
};

void foxtrot::devices::newport2936R::setMode(foxtrot::devices::powermodes mode)
{
  short unsigned sw = static_cast<short unsigned>(mode);
  
  std::ostringstream oss;
  oss << "PM:MODE " << sw <<'\r';
  _proto->write(oss.str());
  

}


void foxtrot::devices::newport2936R::manualTriggerState(bool state)
{
  std::ostringstream oss;
  oss << "PM:TRIG:STATE " << (int) state <<'\r';
  _proto->write(oss.str());
  

}





foxtrot::devices::powerunits convert_string_to_powerunit(std::string s, bool& ok)
{ 
  using foxtrot::devices::powerunits;
  ok = true;
    foxtrot::Logging lg("convert_powerunit_to_string");
  
  lg.Trace("converting string to powerunit");

  
  
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
  
  type::register_converter_func(convert_powerunit_to_string);
  type::register_converter_func(convert_string_to_powerunit);
  type::register_converter_func(convert_int_to_mode);
  type::register_converter_func(convert_mode_to_int);
  
  registration::enumeration<foxtrot::devices::powerunits>("foxtrot::devices::powerunits");
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
  
  
  
}
