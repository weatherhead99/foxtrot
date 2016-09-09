#include "SerialPort.h"
#include "ProtocolError.h"
#include <termios.h>

#include "ProtocolUtilities.h"
#include <fcntl.h>
#include <string.h>
#include <vector>

foxtrot::protocols::SerialPort::SerialPort(const foxtrot::parameterset*const instance_parameters): SerialProtocol(instance_parameters)
{
  

}

foxtrot::protocols::SerialPort::~SerialPort()
{
  //TODO: error checking
  close(_fd);

}


void foxtrot::protocols::SerialPort::Init(const foxtrot::parameterset*const class_parameters)
{
  //call base class to merge parameters together
  CommunicationProtocol::Init(class_parameters);
  
  extract_parameter_value(_port,_params,"port");
  extract_parameter_value(_baudrate,_params,"baudrate");
  
  
  _fd = open(_port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
  
  if(_fd == -1)
  {
    throw ProtocolError("unable to open serial port");
  };
  
  //WARNING: segfault danger!
  termios options;
  tcgetattr(_fd,&options);
  
  //TODO: set attributes
  
  //raw input - see POSIX serial port guide
  options.c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);
  
  
  tcsetattr(_fd,TCSANOW, &options);
  
  
}



std::string foxtrot::protocols::SerialPort::read(unsigned int len)
{
  std::vector<unsigned char> out;
  out.resize(len);
  
  auto ret = ::read(_fd,out.data(),out.size());
  
  if(ret < 0)
  {
    //TODO:appropriate strerror
    throw ProtocolError(std::string("error reading from serial port: ") + strerror(ret) );
  }
  
  //TODO: if number read is incorrect

}

void foxtrot::protocols::SerialPort::write(const std::string& data)
{
  
  auto ret = ::write(_fd,data.data(),data.size());
  if(ret < 0) 
  {
    //TODO: appropriate strerror
    throw ProtocolError(std::string("error writing to serial port: " ) + strerror(ret) );
  }
  
}
