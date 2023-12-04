#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <cctype>

#include <rttr/registration>

#include <foxtrot/ProtocolError.h>
#include <foxtrot/ProtocolTimeoutError.h>
#include <foxtrot/DeviceError.h>

#include <foxtrot/protocols/BulkUSB.h>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/ft_tuple_helper.hh>

#include "newport2936R.h"

using foxtrot::devices::newport2936R;

using foxtrot::devices::powermeterimpedance;
using foxtrot::devices::powermodes;
using foxtrot::devices::powerunits;
using foxtrot::devices::powermeteroutputrange;
using foxtrot::devices::powermeterfiltermodes;
using foxtrot::devices::powermeterexternaltriggermodes;
using foxtrot::devices::powermetertriggerrunmodes;
using foxtrot::devices::powermeteranalogfilterfreq;



#if __GNUC__ > 9
const foxtrot::parameterset newport2936R_usb_params 
{
  {"vid", 0x104d},
  {"pid", 0xcec7},
  {"epin", 0x81},
  {"epout", 0x02}
  
};

const foxtrot::parameterset newport2936R_serial_params
{
  {"baudrate", 38400},
  {"stopbits",  1},
  {"bits",  8},
  {"parity",  "none"},
  {"flowcontrol", "none"}
    
  };
#else
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
#endif



foxtrot::devices::newport2936R::newport2936R(std::shared_ptr< foxtrot::SerialProtocol> proto)
: CmdDevice(proto), _proto(proto), _lg("newport2936R")
{
  
  
  auto specproto = std::dynamic_pointer_cast<foxtrot::protocols::BulkUSB>(proto);
  auto serproto = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(proto);
  if(specproto != nullptr)
  {
    setup_usb_mode();
  }
  else if(  serproto   != nullptr)
  {
    setup_serial_mode(); 
  }
  
  else
  {
      throw DeviceError("invalid protocol for power meter connection!");
  }
}

foxtrot::devices::newport2936R::newport2936R(
    std::shared_ptr<foxtrot::protocols::BulkUSB> proto)
  : CmdDevice(proto), _proto(proto), _lg("newport2936R")
{

  setup_usb_mode();
}

foxtrot::devices::newport2936R::newport2936R(
    std::shared_ptr<foxtrot::protocols::SerialPort> proto)
  : CmdDevice(proto), _proto(proto), _lg("newport2936R")
{

  setup_serial_mode();
}




void foxtrot::devices::newport2936R::setup_usb_mode()
{
  auto specproto = std::static_pointer_cast<foxtrot::protocols::BulkUSB>(_proto);
  
  _lg.Info( "using usb connected power meter");
  specproto->Init(&newport2936R_usb_params);
  _lg.Debug("init done");
  _usbmode = true;
  
  specproto->set_read_timeout(200);
  specproto->set_write_timeout(200);

  
  _usbmode = true;
  cmdptr = &newport2936R::cmd_usb_mode;
  readlineptr = &newport2936R::usb_read_line;
  
}

void foxtrot::devices::newport2936R::setup_serial_mode()
{
  auto serproto = std::static_pointer_cast<foxtrot::protocols::SerialPort>(_proto);
    _lg.Info("using serial connected power meter" );
    serproto->Init(&newport2936R_serial_params);
    _lg.Debug("init done");
    
    _lg.Debug("flushing serial port");
    serproto->flush();
    
    _lg.Debug("disabling command echo");
    serproto->write("ECHO 0\r");
    _usbmode = false;
    cmdptr = &newport2936R::cmd_serial_mode;
    readlineptr = &newport2936R::serial_read_line;
    
    serproto->setWait(200);
    
}

std::string foxtrot::devices::newport2936R::cmd_usb_mode(const std::string& request)
{
  unsigned actlen;
  std::ostringstream oss;
  
  while(true)
    {
      try{
	auto buffer = _proto->read(512, &actlen);
	oss << buffer;
      }
      catch(foxtrot::ProtocolTimeoutError& err)
	{
	  _lg.Error("protocol timeout error caught, breaking");
	  if(_usbmode)
	    {
	      auto specproto = static_cast<foxtrot::protocols::BulkUSB*>(_proto.get());
	      specproto->clear_halts();
	    }
	  break;
	}
      if(actlen < 64)
	{
	  _lg.Trace("received length != 64, breaking");
	  break;
	}	  
    }

  return oss.str();
}


std::string foxtrot::devices::newport2936R::cmd_serial_mode(const std::string& request)
{
  unsigned actlen;
  std::ostringstream oss;

  auto serproto = std::static_pointer_cast<foxtrot::protocols::SerialPort>(_proto);
  serproto->flush();
  
  auto resp = serproto->read_until_endl('\n');

  auto cretpos = std::find(resp.begin(), resp.end(), '\r');
  if(cretpos == resp.end())
    throw foxtrot::DeviceError("failed to find carriage return in device response");
  
  return std::string(resp.begin(), cretpos);

}


std::string foxtrot::devices::newport2936R::cmd(const std::string& request)
{
  cmd_no_response(request);
  return (this->*cmdptr)(request);
}

void foxtrot::devices::newport2936R::cmd_no_response(const std::string& request)
{
  _lg.Debug("request is: " + request);
  _proto->write(request + '\r');
 
};



int foxtrot::devices::newport2936R::getLambda()
{
  return command_get<int>("PM:L?");
}

std::tuple<int,int> newport2936R::getLambdaRange()
{
  auto min = command_get<int>("PM:MIN:L?");
  auto max = command_get<int>("PM:MAX:L?");

  return std::make_tuple(min,max);
}


std::tuple<double,double> newport2936R::getPowerRange()
{
  auto min = command_get<double>("PM:MIN:P?");
  auto max = command_get<double>("PM:MAX:P?");
  return std::make_tuple(min,max);
}


void foxtrot::devices::newport2936R::setLambda(int l)
{
  command_write("PM:L", l);
}

int foxtrot::devices::newport2936R::getErrorCode()
{
  return command_get<int>("ERRORS?");
}

string foxtrot::devices::newport2936R::getErrorString()
{
  return cmd("ERRSTR?");
}


double foxtrot::devices::newport2936R::getPower()
{
  return command_get<double>("PM:P?");
}

void newport2936R::setRange(int r)
{
  if(r < 0 or r >  7)
    throw std::out_of_range("invalid range selection, valid from 0 to 7!");
  command_write("PM:RAN", r);
  
}

int newport2936R::getRange()
{
  return command_get<int>("PM:RAN?");
}

bool newport2936R::getRun()
{
  return command_get<int>("PM:RUN?");
}

void newport2936R::setRun(bool onoff)
{
  command_write("PM:RUN", onoff);
}



double foxtrot::devices::newport2936R::getResponsivity()
{
  return command_get<double>("PM:RESP?");
}

double foxtrot::devices::newport2936R::getArea()
{
  return command_get<double>("PM:DETSIZE?");

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
  command_write("PM:UNIT",static_cast<unsigned>(unit));
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


#ifndef NEW_RTTR_API

foxtrot::devices::powermodes foxtrot::devices::newport2936R::getMode()
{
  auto repl = cmd("PM:MODE?");
  
  powermodes  sw = static_cast<powermodes>(std::stoi(repl));
  return sw;
}
#else

#ifdef linux
    #warning ("WARNING: using hacked functions for custom return types")
#else
    #pragma message ("WARNING: using hacked functions for custom return types")
#endif 
int foxtrot::devices::newport2936R::getMode()
{
  auto repl = cmd("PM:MODE?");
  return std::stoi(repl);
  
};

#endif





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

#ifndef NEW_RTTR_API

void foxtrot::devices::newport2936R::setMode(foxtrot::devices::powermodes mode)
{
  short unsigned sw = static_cast<short unsigned>(mode);
  command_write("PM:MODE",sw);
}
#else
void foxtrot::devices::newport2936R::setMode(int mode)
{
  command_write("PM:MODE",static_cast<short unsigned>(mode));
  
};


#endif


void foxtrot::devices::newport2936R::manualTriggerState(bool state)
{
  command_write("PM:TRIG:STATE", (int) state);
}

bool foxtrot::devices::newport2936R::getTriggerState()
{
  return command_get<int>("PM:TRIG:STATE?");
}


std::string foxtrot::devices::newport2936R::getcaldate()
{
  auto repl = cmd("CALDATE?");
  repl.erase(std::remove_if(repl.begin(), repl.end(), ::isspace),repl.end());
  
  return repl;

}

double foxtrot::devices::newport2936R::getcaltemp()
{
  auto repl = cmd("CALTEMP?");
  return std::stod(repl);
  
}


double foxtrot::devices::newport2936R::getTemperature()
{
  return command_get<double>("PM:TEMP?");
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

powermetertriggerrunmodes foxtrot::devices::newport2936R::getTriggerEndMode()
{
  return command_get<powermetertriggerrunmodes>("PM:TRIG:STOP?");

}


void foxtrot::devices::newport2936R::setTriggerEndMode(powermetertriggerrunmodes mode)
{
  command_write("PM:TRIG:STOP",static_cast<short unsigned>(mode));
 
}

powermetertriggerrunmodes foxtrot::devices::newport2936R::getTriggerStartMode()
{
  return command_get<powermetertriggerrunmodes>("PM:TRIG:START?");
}

void foxtrot::devices::newport2936R::setTriggerStartMode(powermetertriggerrunmodes mode)
{
  command_write("PM:TRIG:START", static_cast<short unsigned>(mode));

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

void foxtrot::devices::newport2936R::setTriggerTimeout(int time_ms)
{
    command_write<int>("PM:TRIG:TIME",time_ms);
}

int foxtrot::devices::newport2936R::getTriggerTimeout()
{
    return command_get<int>("PM:TRIG:TIME?");
}

double foxtrot::devices::newport2936R::getTriggerValue()
{
  return command_get<double>("PM:TRIG:VALUE?");
}

void foxtrot::devices::newport2936R::setTriggerValue(double meas_val)
{
  command_write("PM:TRIG:VALUE",meas_val);

}

string foxtrot::devices::newport2936R::getSerialNumber()
{
  auto repl =  cmd("PM:DETSN?");
  repl.erase(std::remove_if(repl.begin(), repl.end(), ::isspace),repl.end());
  return repl;

}

int foxtrot::devices::newport2936R::getChannel()
{
  return command_get<int>("PM:CHAN?");
}

void foxtrot::devices::newport2936R::setChannel(int chan)
{
  command_write("PM:CHAN", chan);
}

std::array<double, 3> newport2936R::getCorrection()
{
  std::array<double, 3> out;
  auto st = cmd("PM:CORR?");
  std::istringstream iss;

  for(int i=0; i< 3; i++)
    iss >> out[i];
  
  return out;
}

void newport2936R::setCorrection(double d1, double d2, double d3)
{
  std::ostringstream oss;
  oss << "PM:CORR " << d1 <<  " " << d2 <<  " " << d3;
  cmd_no_response(oss.str());
  
}

std::string newport2936R::getDetectorModel()
{
  return cmd("PM:DETMODEL?");
};


powermeteranalogfilterfreq newport2936R::getAnalogFilter()
{
  return command_get<powermeteranalogfilterfreq>("PM:ANALOGFILTER?");

}

void newport2936R::setAnalogFilter(powermeteranalogfilterfreq value)
{
  command_write("PM:ANALOGFILTER", value);
}

powermeterimpedance newport2936R::getOutputImpedance()
{
  return command_get<powermeterimpedance>("PM:ANALOG:IMP?");
}

void newport2936R::setOutputImpedance(powermeterimpedance imp)
{
  command_write("PM:ANALOG:IMP", imp);
}

powermeteroutputrange newport2936R::getOutputRange()
{
  return command_get<powermeteroutputrange>("PM:ANALOG:OUT?");
}

void newport2936R::setOutputRange(powermeteroutputrange range)
{
  command_write("PM:ANALOG:OUT", range);
}

bool newport2936R::getAttenuator(){return std::stoi(cmd("PM:ATT?"));}

std::string newport2936R::getAttenuatorSerialNumber()
{
  return cmd("PM:ATTSN?");
}

bool newport2936R::getAutoRange() { return std::stoi(cmd("PM:AUTO?")); }

void newport2936R::setAutoRange(bool onoff)
{
  command_write("PM:AUTO", onoff);
}


int foxtrot::devices::newport2936R::getDigitalFilter()
{
  return std::stoi(cmd("PM:DIGITALFILTER?"));
}

void foxtrot::devices::newport2936R::setDigitalFilter(int value)
{
  command_write("PM:DIGITALFILTER", value);
}


powermeterfiltermodes newport2936R::getFilterMode()
{
  return command_get<powermeterfiltermodes>("PM:FILTER?");
}

void newport2936R::setFilterMode(powermeterfiltermodes mode)
{
  command_write("PM:FILTER", mode); 
}



bool foxtrot::devices::newport2936R::getBufferBehaviour()
{
    return std::stoi(cmd("PM:DS:BUF?"));
}

void foxtrot::devices::newport2936R::setBufferBehaviour(bool mode)
{
    command_write("PM:DS:BUF", mode);
}

int foxtrot::devices::newport2936R::getDataStoreCount()
{
  return command_get<int>("PM:DS:C?");
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
  command_write("PM:DS:UNIT", units);
}


void foxtrot::devices::newport2936R::clearDataStore()
{
  cmd_no_response("PM:DS:CL");
  
}

int foxtrot::devices::newport2936R::getDataStoreInterval()
{
   return command_get<int>("PM:DS:INT?");
}

void foxtrot::devices::newport2936R::setDataStoreInterval(int interval)
{
  command_write("PM:DS:INT", interval);
}


int foxtrot::devices::newport2936R::getDataStoreSize()
{
  return command_get<int>("PM:DS:SIZE?");
}

void foxtrot::devices::newport2936R::setDataStoreSize(int size)
{
  command_write("PM:DS:SIZE",size);
}

double foxtrot::devices::newport2936R::getDataStoreValue(int idx)
{
   command_write("PM:DS:GET?", idx);
    std::ostringstream oss;
    oss << "PM:DS:GET? " << idx;
    return command_get<double>(oss.str()); 
}


std::vector<double> newport2936R::do_fetch_buffer(int n)
{
    std::vector<double> out;
  out.reserve(n);
  
  bool data_started = false;
  while(true)
    {
      auto line = (this->*readlineptr)();
      if(line.find("Header") != std::string::npos)
	{
	  //this is the end of the header 
	  data_started = true;
	  continue;
	}
      else if(line.find("End") != std::string::npos)
	break;
      if(data_started)
	out.push_back(std::stod(line));
    }

  return out;

}

std::vector<double> foxtrot::devices::newport2936R::fetchDataStore(int begin, int end)
{
  std::ostringstream oss;
  oss << "PM:DS:GET? " <<  begin << "-" << end;
  cmd_no_response(oss.str());
  
  return do_fetch_buffer(end - begin);

}

std::vector< double > foxtrot::devices::newport2936R::fetchDataStoreNewest(int n)
{
  std::ostringstream oss;
  oss <<"PM:DS:GET? "<<  "+" << n;
  cmd_no_response(oss.str());
  return do_fetch_buffer(n);
}

std::vector< double > foxtrot::devices::newport2936R::fetchDataStoreOldest(int n)
{
  std::ostringstream oss;
  oss << "PM:DS:GET? "<< "-" << n;
  cmd_no_response(oss.str());

  return do_fetch_buffer(n);
  
}

// string foxtrot::devices::newport2936R::fetch_store_buffer()
// {
//   std::ostringstream oss;
//   unsigned actlen;
  
//   while(true)
//   {
//     auto buffer = _proto->read(64, &actlen);
//     _lg.strm(sl::debug) << "actlen: " << actlen;
//     oss << std::string(buffer.begin(), buffer.begin() + actlen);
    
//     if(actlen < 64)
//     {
//       break;
//     }
    
//   }
  
//   return oss.str();
  
// }

void foxtrot::devices::newport2936R::flush_buffers_after_timeout(int n)
{
    _lg.Info("flusing powermeter USB buffers");
    unsigned actlen;
    for(int i=0 ; i < n ; i++)
    {
      try{
	_proto->read(64,&actlen);
      }
      catch(foxtrot::ProtocolTimeoutError& err)
      {
	_lg.Trace("caught a timeout error..");
	continue;
      }; 
    }
}


void foxtrot::devices::newport2936R::check_and_throw_error()
{
  int errc = getErrorCode();
    _lg.strm(sl::error) << "caught an error, request, code: " << errc;
    throw DeviceError("powermeter error: " + std::to_string(errc));

}

std::string newport2936R::usb_read_line()
{
  std::string out;
  return _proto->read(512);
}

std::string newport2936R::serial_read_line()
{
  return std::static_pointer_cast<protocols::SerialPort>(_proto)->read_until_endl('\n');
}



std::vector< double > parse_datastore_string(const string& in)
{
  std::istringstream iss(in);
  std::string line;
  
  std::vector<double> out;
  
  while(std::getline(iss,line))
  {
    auto pos = line.find("End");
    if(pos != line.npos)
    {
      break;
    }
  };
  
  
  while(std::getline(iss,line))
  {
    auto pos = line.find("End");
    if(pos != line.npos)
    {
      break;
    }
    
    out.push_back(std::stod(line));
    
  };
  
  return out;
}

std::string foxtrot::devices::newport2936R::info()
{
  return cmd("*IDN?\r");
}



RTTR_REGISTRATION
{
  using namespace rttr;

  foxtrot::register_tuple<std::tuple<int,int>>();
  
  registration::enumeration<powermeterimpedance>("foxtrot::devices::powermeterimpedance")
    (value("_50_ohm", powermeterimpedance::_50_ohm),
     value("_100_kohm", powermeterimpedance::_100_kohm),
     value("_1_megohm", powermeterimpedance::_1_megohm)
     );

  registration::enumeration<powermeteroutputrange>("foxtrot::devices::powermeteroutputrange")
    (value("_1_V", powermeteroutputrange::_1_V),
     value("_2_V", powermeteroutputrange::_2_V),
     value("_5_V", powermeteroutputrange::_5_V),
     value("_10_V", powermeteroutputrange::_10_V));
     
  
  registration::enumeration<powerunits>("foxtrot::devices::powerunits")
  ( value("Amps", powerunits::Amps),
    value("Volts", powerunits::Volts),
    value("Watts", powerunits::Watts),
    value("Watts_cm2", powerunits::Watts_cm2),
    value("Joules", powerunits::Joules),
    value("Joules_cm2", powerunits::Joules_cm2));

  registration::enumeration<powermodes>("foxtrot::devices::powermodes")
    (
     value("DC_cont", powermodes::DC_cont),
     value("DC_sing", powermodes::DC_sing),
     value("Integrate", powermodes::Integrate),
     value("PP_cont", powermodes::PP_cont),
     value("PP_sing", powermodes::PP_sing),
     value("pulse_cont", powermodes::pulse_cont),
     value("pulse_single", powermodes::pulse_single),
     value("RMS", powermodes::RMS));

  registration::enumeration<powermeterfiltermodes>("foxtrot::devices::powermeterfiltermodes")
    (
     value("NO_FILTER", powermeterfiltermodes::NO_FILTER),
     value("ANALOG_FILTER", powermeterfiltermodes::ANALOG_FILTER),
     value("DIGITAL_FILTER", powermeterfiltermodes::DIGITAL_FILTER),
     value("ANALOG_AND_DIGITAL_FILTER", powermeterfiltermodes::ANALOG_AND_DIGITAL_FILTER)
     );

  registration::enumeration<powermeterexternaltriggermodes>("foxtrot::devices::powermeterexternaltriggermodes")
    (
     value("DISABLED", powermeterexternaltriggermodes::DISABLED),
     value("CHANNEL_1", powermeterexternaltriggermodes::CHANNEL_1),
     value("CHANNEL_2", powermeterexternaltriggermodes::CHANNEL_2),
     value("BOTH_CHANNELS", powermeterexternaltriggermodes::BOTH_CHANNELS)
     );
  
  registration::enumeration<powermetertriggerrunmodes>("foxtrot::devices::powermetertriggerrunmodes")
    (
     value("FOREVER", powermetertriggerrunmodes::FOREVER),
     value("EXTERNAL_TRIGGER", powermetertriggerrunmodes::EXTERNAL_TRIGGER),
     value("SOFT_KEY", powermetertriggerrunmodes::SOFT_KEY),
     value("SOFTWARE", powermetertriggerrunmodes::SOFTWARE)

     );

  registration::enumeration<powermeteranalogfilterfreq>("foxtrot::devices::powermeteranalogfilterfreq")
    (
     value("NONE", powermeteranalogfilterfreq::NONE),
     value("_250_kHz", powermeteranalogfilterfreq::_250_kHz),
     value("_12_5_kHz", powermeteranalogfilterfreq::_12_5_kHz),
     value("_1_kHz", powermeteranalogfilterfreq::_1_kHz),
     value("_5_Hz", powermeteranalogfilterfreq::_5_Hz)
     );
  
  
  registration::class_<newport2936R>("foxtrot::devices::newport2936R")
    .property("AnalogFilter", &newport2936R::getAnalogFilter, &newport2936R::setAnalogFilter)
    .property("OutputImpedance", &newport2936R::getOutputImpedance, &newport2936R::setOutputImpedance)
    .property("OutputRange", &newport2936R::getOutputRange, &newport2936R::setOutputRange)
    .property_readonly("Attenuator", &newport2936R::getAttenuator)
    .property_readonly("AttenuatorSerialNumber", &newport2936R::getAttenuatorSerialNumber)
    .property("AutoRange", &newport2936R::getAutoRange, &newport2936R::setAutoRange)
    .property_readonly("caldate",&newport2936R::getcaldate)
    .property_readonly("caltemp",&newport2936R::getcaltemp)
    .property("Channel", &newport2936R::getChannel, &newport2936R::setChannel)
    .method("getCorrection", &newport2936R::getCorrection)
    .method("setCorrection", &newport2936R::setCorrection)(parameter_names("d1", "d2", "d3"))
    .property_readonly("DetectorModel", &newport2936R::getDetectorModel)
    .property_readonly("Area",&newport2936R::getArea)
    .property_readonly("SerialNumber",&newport2936R::getSerialNumber)
    .property("DigitalFilter", &newport2936R::getDigitalFilter, &newport2936R::setDigitalFilter)
    .property("BufferBehaviour", &newport2936R::getBufferBehaviour, &newport2936R::setBufferBehaviour)
    .method("clearDataStore", &newport2936R::clearDataStore)
    .property_readonly("DataStoreCount", &newport2936R::getDataStoreCount)
    .property("DataStoreEnable", &newport2936R::getDataStoreEnable, &newport2936R::setDataStoreEnable)
    .method("getDataStoreValue", &newport2936R::getDataStoreValue)(parameter_names("idx"))
    .method("fetchDataStore", &newport2936R::fetchDataStore)(parameter_names("begin", "end"),
							     metadata("streamdata", true))
    .method("fetchDataStoreOldest", &newport2936R::fetchDataStoreOldest)(parameter_names("n"),
									 metadata("streamdata", true))
    .method("fetchDataStoreNewest", &newport2936R::fetchDataStoreNewest)(parameter_names("n"),
									 metadata("streamdata", true))
    .property("DataStoreInterval", &newport2936R::getDataStoreInterval, &newport2936R::setDataStoreInterval)
    .property("DataStoreSize", &newport2936R::getDataStoreSize, &newport2936R::setDataStoreSize)
    .property("DataStoreUnits", &newport2936R::getDataStoreUnits, &newport2936R::setDataStoreUnits)
    .property("FilterMode", &newport2936R::getFilterMode, &newport2936R::setFilterMode)
    .property("Lambda",&newport2936R::getLambda, &newport2936R::setLambda)
    .property_readonly("LambdaRange", &newport2936R::getLambdaRange)
    .property_readonly("PowerRange", &newport2936R::getPowerRange)
    .property("Mode", &newport2936R::getMode, &newport2936R::setMode)    
    .property_readonly("Power",&newport2936R::getPower)
    .property("Range", &newport2936R::getRange, &newport2936R::setRange)
    .property_readonly("Responsivity", &newport2936R::getResponsivity)
    .property("Run", &newport2936R::getRun, &newport2936R::setRun)
    .property_readonly("Temperature", &newport2936R::getTemperature)
    .property("ExternalTriggerMode", &newport2936R::getExternalTriggerMode,
	      &newport2936R::setExternalTriggerMode)
    .property("TriggerEdge", &newport2936R::getTriggerEdge, &newport2936R::setTriggerEdge)

    
    .property("TriggerStartMode", &newport2936R::getTriggerStartMode,
	      &newport2936R::setTriggerStartMode)
    .property("TriggerEndMode", &newport2936R::getTriggerEndMode,
	      &newport2936R::setTriggerEndMode)
    .property_readonly("TriggerState", &newport2936R::getTriggerState)
    .method("manualTriggerState", &newport2936R::manualTriggerState)(parameter_names("state"))
    .property("TriggerValue", &newport2936R::getTriggerValue, &newport2936R::setTriggerValue)
    .property("TriggerTimeout", &newport2936R::getTriggerTimeout, &newport2936R::setTriggerTimeout)
    .property("Units",&newport2936R::getUnits, &newport2936R::setUnits)









  
  ;
  
  
  
}
