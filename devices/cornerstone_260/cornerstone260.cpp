#include "cornerstone260.h"
#include "ProtocolError.h"
#include "ProtocolUtilities.h"
#include "SerialPort.h"

#include <map>
#include <memory>
#include <type_traits>

#include <sstream>

const foxtrot::parameterset cornerstone_class_params_serial
{
  {"baudrate" , 9600},
  {"parity", "none"},
  {"stopbits", 1},
  {"flowcontrol", "none"},
  {"bits", 8}
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
  

}


std::string foxtrot::devices::cornerstone260::cmd(const std::string& request)
{
  
  _serproto->write(request + '\n');
  
  //read echo, and throw away
  unsigned actlen;
  
  if(_cancelecho)
  {
    _serproto->read( request.size() + 1, &actlen );
    if(actlen < (request.size() + 1) )
    {
      throw ProtocolError("couldn't read cornerstone echo");
    }
  }

  auto response = read_until_endl(_serproto.get(),20,'\n'); 
  
  
  

}

