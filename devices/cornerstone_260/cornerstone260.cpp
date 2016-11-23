#include "cornerstone260.h"
#include "ProtocolError.h"
#include "ProtocolUtilities.h"
#include "SerialPort.h"

#include <map>
#include <memory>
#include <type_traits>

#include <sstream>

#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>

const foxtrot::parameterset cornerstone_class_params_serial
{
  {"baudrate" , 9600u},
  {"parity", "none"},
  {"stopbits", 1u},
  {"flowcontrol", "none"},
  {"bits", 8u}
};

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




foxtrot::devices::cornerstone260::cornerstone260(std::shared_ptr< foxtrot::SerialProtocol> proto)
: CmdDevice(proto), _serproto(proto)
{
  
  auto serportptr = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(proto);
  if(serportptr == nullptr)
  {
    throw ProtocolError("only SerialPort connection currently supported");
  };
  
  //initialize communication port
  _cancelecho = true;
  serportptr->Init(&cornerstone_class_params_serial);
  serportptr->flush();
  
}


std::string foxtrot::devices::cornerstone260::cmd(const std::string& request)
{
  
  auto serportptr = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(_serproto);

  if(serportptr == nullptr)
    {
      throw ProtocolError("only serial port supported at the moment");
    }
   else
   {
    serportptr->flush(); 
     
   }
      

  
  
//   std::cout << "request size: " << request.size() << std::endl;
  
  _serproto->write(request + '\n');
  
  //read echo, and throw away
 
  unsigned actlen;
  
//   int timeneeded = std::round(1./9600 * request.size() * 1000);
  

//   std::this_thread::sleep_for(std::chrono::milliseconds(2*timeneeded));
  
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  
//   std::cout << "bytes available: " << serportptr->bytes_available() << std::endl;

//   std::cout << "reading echo" << std::endl;
  
  auto echo = readecho(request);

//   std::cout << "echo ok" << std::endl;
//   std::cout << echo << std::endl;
  
//   std::cout << "bytes available: " << serportptr->bytes_available() << std::endl;

  //HACK: if no bytes available, assume no response
  if( serportptr->bytes_available() == 0)
  {
    return "";
  }
  
  auto response = serportptr->read_until_endl('\r');

  auto cretpos = std::find(response.begin(),response.end(),'\r');
 
  return std::string(response.begin(),cretpos );

}

std::string foxtrot::devices::cornerstone260::readecho(const std::string& request)
{
  unsigned actlen;
  auto echo = _serproto->read(request.size() +1, &actlen);
  
  //echo might not be equal to request
  auto newlpos = std::find(echo.begin(),echo.end(),'\n');
  
  
  if( newlpos == echo.end())
    {
      std::cout << "echo: " << echo << std::endl;
      
      for (auto& c : echo)
      {
	std::cout << (int) c << " ";
      }
      std::cout << std::endl;
      
      throw ProtocolError("couldn't read echo properly");
    }
  if( std::string(echo.begin(),echo.end()-1) != request)
    {
      
      std::cout << "echo: " << echo << std::endl;
      
      for (auto& c : echo)
      {
	std::cout << (int) c << " ";
      }
      std::cout << std::endl;
      
      
      throw ProtocolError("echo was invalid");
    };

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
  
  oss << "GOWAVE " << std::setprecision(2) << std::fixed << wl_nm;
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


