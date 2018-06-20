#include "newport2936R.h"
#include "BulkUSB.h"
#include "SerialPort.h"
#include <iostream>
#include "DeviceError.h"
#include "ProtocolError.h"
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
  
  else
  {
      throw DeviceError("invalid protocol for power meter connection!");
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

bool foxtrot::devices::newport2936R::getTriggerState()
{
  auto repl = cmd("PM:TRIG:STATE?");
  return std::stoi(repl);

}


std::string foxtrot::devices::newport2936R::getcaldate()
{
  auto repl = cmd("CALDATE?");
  return repl;

}

double foxtrot::devices::newport2936R::getcaltemp()
{
  auto repl = cmd("CALTEMP?");
  return std::stod(repl);
  
}


double foxtrot::devices::newport2936R::getTemperature()
{
  auto repl = cmd("PM:TEMP?");
  return std::stod(repl);
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

int foxtrot::devices::newport2936R::getExternalTriggerMode()
{
  auto repl = cmd("PM:TRIG:EXT?");
  return std::stoi(repl);

}

void foxtrot::devices::newport2936R::setExternalTriggerMode(int mode)
{
  std::ostringstream oss;
  oss << "PM:TRIG:EXT " << mode <<'\r';
  _proto->write(oss.str());
}

int foxtrot::devices::newport2936R::getTriggerEndMode()
{
  auto repl = cmd("PM:TRIG:STOP?");
  return std::stoi(repl);

}

void foxtrot::devices::newport2936R::setTriggerEndMode(int mode)
{
  std::ostringstream oss;
  oss << "PM:TRIG:STOP " << mode <<'\r';
  _proto->write(oss.str());
}

int foxtrot::devices::newport2936R::getTriggerStartMode()
{
  auto repl = cmd("PM:TRIG:START?");
  return std::stoi(repl);

}

void foxtrot::devices::newport2936R::setTriggerStartMode(int mode)
{
  std::ostringstream oss;
  oss << "PM:TRIG:START " << mode <<'\r';
  _proto->write(oss.str());

}

int foxtrot::devices::newport2936R::getTriggerEdge()
{
  auto repl = cmd("PM:TRIG:EDGE?");
  return std::stoi(repl);
}

void foxtrot::devices::newport2936R::setTriggerEdge(int edge)
{
  std::ostringstream oss;
  oss << "PM:TRIG:EDGE " << edge << '\r';
  _proto->write(oss.str());

}

string foxtrot::devices::newport2936R::getSerialNumber()
{
  return cmd("PM:DETSN?");

}

int foxtrot::devices::newport2936R::getChannel()
{
  return std::stoi(cmd("PM:CHAN?"));

}

void foxtrot::devices::newport2936R::setChannel(int chan)
{
  std::ostringstream oss;
  oss << "PM:CHAN " << chan  <<'\r' ;
  _proto->write(oss.str());
}


int foxtrot::devices::newport2936R::getAnalogFilter()
{
  return std::stoi(cmd("PM:ANALOGFILTER?"));

}

void foxtrot::devices::newport2936R::setAnalogFilter(int value)
{
  std::ostringstream oss;
  oss << "PM:ANALOGFILTER " << value <<'\r';
  _proto->write(oss.str());
}


int foxtrot::devices::newport2936R::getDigitalFilter()
{
  return std::stoi(cmd("PM:DIGITALFILTER?"));
}

void foxtrot::devices::newport2936R::setDigitalFilter(int value)
{
  std::ostringstream oss;
  oss << "PM:DIGITALFILTER " << value <<'\r';
  _proto->write(oss.str());
}


int foxtrot::devices::newport2936R::getFilterMode()
{
  return std::stoi(cmd("PM:FILTER?"));

}

void foxtrot::devices::newport2936R::setFilterMode(int mode)
{
  std::ostringstream oss;
  oss << "PM:FILTER " << mode  <<'\r';
  _proto->write(oss.str());
  
}

bool foxtrot::devices::newport2936R::getBufferBehaviour()
{
    return std::stoi(cmd("PM:DS:BUF?"));
}

void foxtrot::devices::newport2936R::setBufferBehaviour(bool mode)
{
    std::ostringstream oss;
    oss << "PM:DS:BUF " << mode << '\r';
    _proto->write(oss.str());
}

int foxtrot::devices::newport2936R::getDataStoreCount()
{
    return std::stoi(cmd("PM:DS:C?"));
}

bool foxtrot::devices::newport2936R::getDataStoreEnable()
{
    return std::stoi(cmd("PM:DS:EN?"));
}

void foxtrot::devices::newport2936R::setDataStoreEnable(bool onoff)
{
  command_write("PM:DS:EN",onoff);
  
}

foxtrot::devices::powerunits foxtrot::devices::newport2936R::getDataStoreUnits()
{
    auto str = cmd("PM:DS:UNIT?");
    bool ok;
    auto out = convert_string_to_powerunit(str,ok);
    
    if(!ok)
    {
        std::string msg( "couldn't convert string to power unit: ");
        msg += str;
        throw DeviceError(msg);
    }
        
    //TODO: error reporting here!
    return out;    
}

void foxtrot::devices::newport2936R::setDataStoreUnits(foxtrot::devices::powerunits units)
{
    _proto->write(std::string("PM:DS:UNIT ") + std::to_string(static_cast<unsigned>(units))+'\r');
}


void foxtrot::devices::newport2936R::clearDataStore()
{
  _proto->write("PM:DS:CL");
}

int foxtrot::devices::newport2936R::getDataStoreInterval()
{
    return std::stoi(cmd("PM:DS:INT?"));
}

void foxtrot::devices::newport2936R::setDataStoreInterval(int interval)
{
    std::ostringstream oss;
    oss << "PM:DS:INT " << interval <<'\r';
    _proto->write(oss.str());
}


int foxtrot::devices::newport2936R::getDataStoreSize()
{
    return std::stoi(cmd("PM:DS:SIZE?"));
}

void foxtrot::devices::newport2936R::setDataStoreSize(int size)
{
    std::ostringstream oss;
    oss << "PM:DS:SIZE " << size << '\r';
    _proto->write(oss.str());
}

double foxtrot::devices::newport2936R::getDataStoreValue(int idx)
{
    std::ostringstream oss;
    oss << "PM:DS:GET? " << idx << '\r';
    return std::stod(cmd(oss.str()));    
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
  .property("Lambda",&newport2936R::getLambda, &newport2936R::setLambda)
  .property_readonly("Power",&newport2936R::getPower)
  .property_readonly("Responsivity", &newport2936R::getResponsivity)
  .property_readonly("Area",&newport2936R::getArea)
  .property("Units",&newport2936R::getUnits, &newport2936R::setUnits)
  .property("Mode", &newport2936R::getMode, &newport2936R::setMode)
  .method("manualTriggerState", &newport2936R::manualTriggerState)(parameter_names("state"))
  .property_readonly("TriggerState", &newport2936R::getTriggerState)
  .property_readonly("caldate",&newport2936R::getcaldate)
  .property_readonly("caltemp",&newport2936R::getcaltemp)
  .property_readonly("Temperature", &newport2936R::getTemperature)
  .property_readonly("SerialNumber",&newport2936R::getSerialNumber)
  .property("TriggerStartModMode", &newport2936R::getTriggerStartMode,
            &newport2936R::setTriggerStartMode)
  .property("TriggerEndMode", &newport2936R::getTriggerEndMode,
            &newport2936R::setTriggerEndMode)
  .property("ExternalTriggerMode", &newport2936R::getExternalTriggerMode,
            &newport2936R::setExternalTriggerMode)
  .property("TriggerEdge", &newport2936R::getTriggerEdge, &newport2936R::setTriggerEdge)
  .property("Channel", &newport2936R::getChannel, &newport2936R::setChannel)
  .property("AnalogFilter", &newport2936R::getAnalogFilter, &newport2936R::setAnalogFilter)
  .property("DigitalFilter", &newport2936R::getDigitalFilter, &newport2936R::setDigitalFilter)
  .property("FilterMode", &newport2936R::getFilterMode, &newport2936R::setFilterMode)
  .method("clearDataStore", &newport2936R::clearDataStore)
  .property_readonly("DataStoreCount", &newport2936R::getDataStoreCount)
  .property("BufferBehaviour", &newport2936R::getBufferBehaviour,
            &newport2936R::setBufferBehaviour)
  
  ;
  
  
  
}
