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




foxtrot::devices::cornerstone260::cornerstone260(std::shared_ptr<SerialPort> proto)
  : CmdDevice(proto),  _lg("cornerstone260")
{

  
  //initialize communication port
  _cancelecho = true;
  proto->Init(&cornerstone_class_params_serial);
  proto->flush();
  
}


std::string foxtrot::devices::cornerstone260::cmd(const std::string& request)
{

  auto serportptr = std::static_pointer_cast<SerialPort>(_proto);

  serportptr->flush();
      
  auto timeout = serportptr->calc_minimum_transfer_time(request.size() + 1) * 5;
  serportptr->setWait(timeout.count());
  serportptr->write(request + '\n');
  
  auto echo = readecho(request);

  serportptr->setWait(2000);
  auto response = serportptr->read_until_endl('\r');
  auto cretpos = std::find(response.begin(),response.end(),'\r');
 
  return std::string(response.begin(),cretpos);

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
  cmd(request + cmdch);
  
}

void foxtrot::devices::cornerstone260::setWave(double wl_nm)
{
 
  std::ostringstream oss;
  
  oss << "GOWAVE " << std::setprecision(6) << std::fixed << wl_nm;
//   std::cout << "sending command: " << oss.str()<< std::endl;
  
  cmd(oss.str());
  

}


double foxtrot::devices::cornerstone260::getWave()
{
  std::string command{"WAVE?"};
  
//   std::cout << "sending command: " << command << std::endl;
  auto repl = cmd(command);
//   std::cout << "repl is: " << repl << std::endl;
  
  
  return std::stod(repl);

}

void foxtrot::devices::cornerstone260::setFilter(int fnum)
{
  std::ostringstream oss;
  oss << "FILTER " << fnum ;
  cmd(oss.str());
}

int foxtrot::devices::cornerstone260::getFilter()
{
  std::string command("FILTER?");
  auto repl = cmd(command);
  
  return std::stoi(repl);
  
}


int foxtrot::devices::cornerstone260::getGrating()
{
  std::string command("GRAT?");
  auto repl = cmd(command);
  
  return std::stoi(repl);

}



void foxtrot::devices::cornerstone260::setGrating(int gr)
{
  
  std::ostringstream oss ;
  oss << "GRAT " << gr ;
  cmd(oss.str());
  
}

void foxtrot::devices::cornerstone260::setGratingCalibration(int gr, int lines, double factor, double zero, double offset, std::string label)
{
  label = label.substr(0,9);
  std::ostringstream oss;
  oss << "GRAT" << gr << "LINES " << lines;
  cmd(oss.str());
  
  oss.str("");
  oss << "GRAT" << gr << "FACTOR " << std::setprecision(6) << factor;
  cmd(oss.str());
  oss.str("");
  
  
  oss << "GRAT" << gr << "OFFSET " << std::setprecision(6) << offset;
  cmd(oss.str());
  oss.str("");
  
  oss << "GRAT" << gr << "ZERO " << std::setprecision(6) << zero;
  cmd(oss.str());
  oss.str("");
  
  oss << "GRAT" << gr <<"LABEL " << label;
  cmd(oss.str());
  

}


void foxtrot::devices::cornerstone260::abort()
{
  cmd("ABORT");
}

std::string foxtrot::devices::cornerstone260::lasterror()
{
  auto repl = cmd("ERROR?");
  auto errcode = std::stoul(repl);
  try
    {
      return cornerstone_error_strings.at(errcode);
    }
  catch(std::out_of_range& err)
    {
      std::ostringstream oss;
      oss << "unknown error, code: " << std::dec << errcode;
      return oss.str();
    }
  
  
}


bool foxtrot::devices::cornerstone260::errstatus()
{
  auto repl = cmd("STB?");
  return static_cast<bool>(std::stoul(repl));
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
    ;
  
  
  
}

