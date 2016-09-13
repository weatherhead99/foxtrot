#include "SerialPort.h"
#include "ProtocolError.h"
#include <termios.h>

#include "ProtocolUtilities.h"
#include <fcntl.h>
#include <string.h>
#include <vector>

#include <map>

const std::map<unsigned,unsigned> baudrates = { 
  {0 , B0} ,
  {50, B50},
  {75, B75},
  {110, B110},
  {134, B134},
  {150, B150},
  {200, B200},
  {300, B300},
  {600, B600},
  {1200, B1200},
  {1800, B1800},
  {2400, B2400},
  {4800, B4800},
  {9600, B9600},
  {19200, B19200},
  {38400, B38400},
  {57600, B57600},
  {115200, B115200},
  {230400, B230400},
  {460800, B460800},
  {500000, B500000},
  {576000, B576000},
  {921600, B921600},
  {1000000, B1000000},
  {1152000, B1152000},
  {1500000, B1500000},
  {2000000, B2000000},
  {2500000, B2500000},
  {3000000, B3000000},
  {3500000, B3500000},
  {4000000, B4000000}
  
};




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

  //match baudrate to allowed values
  
  unsigned brate;
  try{
    brate = baudrates.at(_baudrate);
  }
  catch(std::out_of_range)
  {
   throw ProtocolError("invalid baud rate"); 
  }
  

  
  extract_parameter_value(_parity,_params,"parity",false);
  extract_parameter_value(_stopbits,_params,"stopbits",false);
  extract_parameter_value(_timeout,_params,"timeout",false);
  
  std::cout << "opening fd.. " << _port <<  std::endl;
  _fd = open(_port.c_str(), O_RDWR |  O_NOCTTY  );
  
  if(_fd == -1)
  {
    throw ProtocolError("unable to open serial port");
  };
  
  
  std::cout << "setting options... " << std::endl;
  
  //WARNING: segfault danger!
  termios options;
  auto ret = tcgetattr(_fd,&options);
  if(ret < 0)
  {
    throw ProtocolError(std::string("couldn't get attributes for tty:") +  strerror(ret));
  }
  
  
  //TODO: set attributes
  options.c_cflag = CS8 | CREAD | ~CRTSCTS;
  
  
  
  //raw input - see POSIX serial port guide
//   options.c_lflag = ICANON;
  
  //set blocking read
  options.c_cc[VMIN] =  1;
  options.c_cc[VTIME] = _timeout;
  
  ret = tcflush(_fd,TCIFLUSH);
  if( ret <0)
  {
    throw ProtocolError(std::string("unable to flush serial port: ") + strerror(ret));
  };
  
  
  
  ret = cfsetispeed(&options,brate);
  if(ret < 0 )
  {
    throw ProtocolError( std::string("couldn't set i speed: ") + strerror(ret)) ;
  }
  
  ret = cfsetospeed(&options,brate);
  if(ret < 0)
  {
    throw ProtocolError( std::string("couldn't set o speed: " )+  strerror(ret) );
  }

  
  ret = tcsetattr(_fd,TCSANOW, &options);
  if(ret <0)
  {
    throw ProtocolError(std::string("couldn't set tty attributes: " ) + strerror(ret));
  }
  
  
  
  
}



std::string foxtrot::protocols::SerialPort::read(unsigned int len)
{
  std::vector<unsigned char> out;
  out.resize(len);
  
  
  auto ret = ::read(_fd,out.data(),out.size());
  
  if(ret < 0)
  {
    
    //TODO:appropriate strerror
    throw ProtocolError(std::string("error reading from serial port: ") + strerror(errno) );
  }
  
  //TODO: if number read is incorrect

}

void foxtrot::protocols::SerialPort::write(const std::string& data)
{
  
  auto ret = ::write(_fd,data.data(),data.size());
  if(ret < 0) 
  {
    //TODO: appropriate strerror
    throw ProtocolError(std::string("error writing to serial port: " ) + strerror(errno) );
  }
  
}
