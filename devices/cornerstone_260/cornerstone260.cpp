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

  std::cout << "request size: " << request.size() << std::endl;

  _serproto->write(request + '\n');
  
  //read echo, and throw away
 
  unsigned actlen;

  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  auto serportptr = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(_serproto);

  if(serportptr == nullptr)
    {
      throw ProtocolError("only serial port supported at the moment");
    };

  std::cout << "bytes available: " << serportptr->bytes_available() << std::endl;

  std::cout << "reading echo" << std::endl;
  
  readecho(request);

  std::cout << "echo ok" << std::endl;

  std::cout << "bytes available: " << serportptr->bytes_available() << std::endl;

  auto response = serportptr->read_until_endl('\r');

  auto cretpos = std::find(response.begin(),response.end(),'\r');
 
  return std::string(response.begin(),cretpos );

}

void foxtrot::devices::cornerstone260::readecho(const std::string& request)
{
  unsigned actlen;
  auto echo = _serproto->read(request.size() +1, &actlen);
  if( echo[echo.size()-1] != '\n')
    {
      throw ProtocolError("couldn't read echo properly");
    }
  if( std::string(echo.begin(),echo.end()-1) != request)
    {
      throw ProtocolError("echo was invalid");
    };


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
