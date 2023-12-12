#include <map>
#include <memory>
#include <type_traits>
#include <sstream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <iostream>

#include <rttr/registration>

#include <foxtrot/ProtocolError.h>
#include <foxtrot/DeviceError.h>

#include <foxtrot/protocols/ProtocolUtilities.h>
#include <foxtrot/protocols/SerialPort.h>

#include "cornerstone260.h"

using foxtrot::protocols::SerialPort;

//WARNING: unsigned_signed_parameter_bug
#if __GNUC__ > 9
const foxtrot::parameterset cornerstone_class_params_serial
{
  {"baudrate" , 9600},
  {"parity", "none"},
  {"stopbits", 1},
  {"flowcontrol", "none"},
  {"bits", 8}
};
#else
const foxtrot::parameterset cornerstone_class_params_serial
{
  {"baudrate" , 9600u},
  {"parity", "none"},
  {"stopbits", 1u},
  {"flowcontrol", "none"},
  {"bits", 8u}
};
#endif

const std::map<unsigned char, std::string> cornerstone_error_strings
{
  {1, "Command not understood"},
  {2, "Bad parameter used in Command"},
  {3, "Destination Position for wavelength motion not allowed"},
  {6, "Accessory not present (usually filter wheel)"},
  {7, "Accessory already in specified position"},
  {8, "Could not home wavelength drive"},
  {9, "Label too long"},
  {0, "system error (miscellaneous)"}
};


struct SerialPortWaitSetter
{
  SerialPortWaitSetter(std::shared_ptr<foxtrot::CommunicationProtocol> proto,
		       unsigned wait_ms)
  {
    _proto = std::static_pointer_cast<foxtrot::protocols::SerialPort>(proto);
    storedwait = _proto->getWait();
    _proto->setWait(wait_ms);
  }

  ~SerialPortWaitSetter()
  {
    _proto->setWait(storedwait);
  }
  
  
  std::shared_ptr<foxtrot::protocols::SerialPort> _proto;
  unsigned storedwait;
};



foxtrot::devices::cornerstone260::cornerstone260(std::shared_ptr<SerialPort> proto)
  : CmdDevice(proto),  _lg("cornerstone260")
{

  
  //initialize communication port
  _cancelecho = true;
  proto->Init(&cornerstone_class_params_serial);
  proto->flush();
  proto->setWait(2000);
}


std::string foxtrot::devices::cornerstone260::cmd(const std::string& request)
{

  cmd_no_response(request);

  
  auto serportptr = std::static_pointer_cast<SerialPort>(_proto);

  _lg.strm(sl::trace) << "reading response";
  auto response = serportptr->read_until_endl('\n');
  auto cretpos = std::find(response.begin(),response.end(),'\r');

  if(cretpos == response.end())
    throw foxtrot::DeviceError("failed to find carriage return in device response");
 
  return std::string(response.begin(),cretpos);

}

void foxtrot::devices::cornerstone260::cmd_no_response(const std::string& request)
{
  
  auto serportptr = std::static_pointer_cast<SerialPort>(_proto);
  _lg.strm(sl::trace) << "flushing port..";
  serportptr->flush();

  _lg.strm(sl::trace) << "writing request";
  serportptr->write(request + '\n');

  _lg.strm(sl::trace) << "reading echo";
  auto echo = readecho(request);
  _lg.strm(sl::trace) << "echo is: " << echo;

}

void foxtrot::devices::cornerstone260::err_check()
{

  auto errstat = errstatus();
  if(errstat)
    {
      auto errstr = lasterror();
      throw foxtrot::DeviceError(errstr);
    }

 
}



const std::string foxtrot::devices::cornerstone260::getDeviceTypeName() const
{
    return "cornerstone260";
}


std::string foxtrot::devices::cornerstone260::readecho(const std::string& request)
{
  unsigned actlen;

  auto serportptr = std::static_pointer_cast<SerialPort>(_proto);
  auto timeout = serportptr->calc_minimum_transfer_time(request.size() +1) * 5;
  auto echo = serportptr->read_until_endl('\n');
  echo.erase(std::remove(echo.begin(), echo.end(), '\n'));
  
  if(echo.size() != request.size())
    {
      _lg.strm(sl::error) << "size of echo: " << echo.size() << " expected size: " << request.size();
    throw ProtocolError("invalid size of echo");
    }

  return echo;

}


bool foxtrot::devices::cornerstone260::getShutterStatus()
{
  auto response = cmd("SHUTTER?");

  err_check();
  
  switch(response[0])
    {
    case 'C':
      return false;
    case 'O':
      return true;
    default:
      throw ProtocolError("invalid shutter status");
    }


}

void foxtrot::devices::cornerstone260::setShutterStatus(bool status)
{
  char cmdch = status ? 'O' : 'C';
  
  std::string request{"SHUTTER "};
  cmd_no_response(request + cmdch);
  err_check();
  
}

void foxtrot::devices::cornerstone260::setWave(double wl_nm)
{
 
  std::ostringstream oss;
  
  oss << "GOWAVE " << std::setprecision(6) << std::fixed << wl_nm;
//   std::cout << "sending command: " << oss.str()<< std::endl;

  {
    SerialPortWaitSetter waiter(_proto, 120000);
    cmd_no_response(oss.str());
    err_check();
  }


}


double foxtrot::devices::cornerstone260::getWave()
{
  std::string command{"WAVE?"};
  
//   std::cout << "sending command: " << command << std::endl;
  auto repl = cmd(command);
//   std::cout << "repl is: " << repl << std::endl;
  

  err_check();
  
  return std::stod(repl);

}

void foxtrot::devices::cornerstone260::setFilter(int fnum)
{
  std::ostringstream oss;
  oss << "FILTER " << fnum ;

  SerialPortWaitSetter waiter(_proto, 120000);
  cmd_no_response(oss.str());
}

int foxtrot::devices::cornerstone260::getFilter()
{
  std::string command("FILTER?");
  auto repl = cmd(command);
  err_check();
  
  return std::stoi(repl);
  
}


int foxtrot::devices::cornerstone260::getGrating()
{
  std::string command("GRAT?");
  auto repl = cmd(command);
  err_check();
  
  return std::stoi(repl);

}



void foxtrot::devices::cornerstone260::setGrating(int gr)
{
  
  std::ostringstream oss ;
  oss << "GRAT " << gr ;

  //NOTE: this can take a LOT of time to come back....
  SerialPortWaitSetter waiter(_proto, 120000);
  cmd_no_response(oss.str());
  err_check();
  
}

void foxtrot::devices::cornerstone260::setGratingCalibration(int gr, int lines, double factor, double zero, double offset, std::string label)
{
  label = label.substr(0,9);

  _lg.strm(sl::trace) << "LINES";
  
  std::ostringstream oss;
  oss << "GRAT" << gr << "LINES " << lines;
  cmd_no_response(oss.str());
  err_check();

  

  _lg.strm(sl::trace) << "FACTOR";
  oss.str("");
  oss << "GRAT" << gr << "FACTOR " << std::setprecision(6) << factor;
  cmd_no_response(oss.str());
  err_check();
  oss.str("");
  

  _lg.strm(sl::trace) << "OFFSET";
  oss << "GRAT" << gr << "OFFSET " << std::setprecision(6) << offset;
  cmd_no_response(oss.str());
  err_check();
  oss.str("");

  _lg.strm(sl::trace) << "ZERO";
  oss << "GRAT" << gr << "ZERO " << std::setprecision(6) << zero;
  cmd_no_response(oss.str());
  err_check();
  oss.str("");


  _lg.strm(sl::trace) << "LABEL";
  oss << "GRAT" << gr <<"LABEL " << label;
  cmd_no_response(oss.str());
  err_check();

}


void foxtrot::devices::cornerstone260::abort()
{
  cmd_no_response("ABORT");
  err_check();
}

std::string foxtrot::devices::cornerstone260::lasterror()
{
  auto repl = cmd("ERROR?");
  unsigned short errcode;

  if(repl.size() == 0)
    {
      throw std::runtime_error("no error string stored");
    }

  try{
    errcode = std::stoul(repl);
       }
  catch(std::exception& err)
       {
	 std::ostringstream oss;
	 oss << "cornerstone260 unknown error, raw: " << repl;
	 return oss.str();
       }
   
  try
    {
      return cornerstone_error_strings.at(errcode);
    }
  catch(std::out_of_range& err)
    {
      std::ostringstream oss;
      oss << "cornerstone260 unknown error, code: " << std::dec << errcode;
      return oss.str();
    }
  
  
}


bool foxtrot::devices::cornerstone260::errstatus()
{
  auto repl = cmd("STB?");
  return static_cast<bool>(std::stoul(repl));
}

std::string foxtrot::devices::cornerstone260::info()
{
  return cmd("INFO?");
  err_check();
}


RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::cornerstone260;


  //deprecate this (painful I know!)
  // registration::class_<cornerstone260>("foxtrot::devices::cornerstone260")
  // .property_readonly("getShutterStatus",&cornerstone260::getShutterStatus)
  //   .method("setShutterStatus",&cornerstone260::setShutterStatus)
  // .property_readonly("getWave",&cornerstone260::getWave)
  // .property_readonly("getFilter",&cornerstone260::getFilter)
  // .property_readonly("getGrating", &cornerstone260::getGrating)
  // .method("setWave",&cornerstone260::setWave)
  // (parameter_names("wl_nm"))
  // .method("setFilter",&cornerstone260::setFilter)
  // (parameter_names("fl"))
  // .method("setGrating",&cornerstone260::setGrating)
  // (parameter_names("gr"))
  // .method("setGratingCalibration", &cornerstone260::setGratingCalibration)
  // (parameter_names("gr","lines","factor","zero","offset","label"))
  // ;


  registration::class_<cornerstone260>("foxtrot::devices::cornerstone260")
    .property("ShutterStatus", &cornerstone260::getShutterStatus,
	      &cornerstone260::setShutterStatus)
    .property("Wave", &cornerstone260::getWave,
	      &cornerstone260::setWave)
    .property("Grating", &cornerstone260::getGrating,
	      &cornerstone260::setGrating)
    .property("Filter", &cornerstone260::getFilter,
	      &cornerstone260::setFilter)
    .method("setGratingCalibration", &cornerstone260::setGratingCalibration)
    (parameter_names("gr", "lines", "factor", "zero", "offset", "label"))
    .property_readonly("errstatus", &cornerstone260::errstatus)
    .property_readonly("lasterror", &cornerstone260::lasterror)
    .method("abort", &cornerstone260::abort)
    .property_readonly("info", &cornerstone260::info)
    ;
  
  
  
}

