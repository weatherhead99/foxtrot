#include "SerialPort.h"
#include "ProtocolError.h"
#include <termios.h>

#include "ProtocolUtilities.h"

#include <fcntl.h>
#include <string.h>
#include <vector>

#include <iostream>
#include <map>

#include <chrono>
#include <thread>

#include <algorithm>

using namespace boost::asio;
using boost::asio::serial_port_base;


const std::map<unsigned, serial_port_base::stop_bits::type> sbitmap 
{ {1, serial_port_base::stop_bits::one },
  {2, serial_port_base::stop_bits::two}
};

const std::map<std::string, serial_port_base::parity::type> paritymap
{
  {"none" , serial_port_base::parity::none},
  {"even" , serial_port_base::parity::even},
  {"odd", serial_port_base::parity::odd}
  
};

const std::map<std::string, serial_port_base::flow_control::type> flowmap
{
  {"none" , serial_port_base::flow_control::none},
  {"hardware", serial_port_base::flow_control::hardware},
  {"software", serial_port_base::flow_control::software}
  
};


foxtrot::protocols::SerialPort::SerialPort(const foxtrot::parameterset*const instance_parameters)
: SerialProtocol(instance_parameters), _sport(_io_service)
{
  

}

foxtrot::protocols::SerialPort::~SerialPort()
{
  //TODO: error checking
  boost::system::error_code ec;
  _sport.close(ec);
  
  if(ec)
  {
    std::cout << "unable to close serial port: " << ec.message() << std::endl;
  };
  
  

}


void foxtrot::protocols::SerialPort::Init(const foxtrot::parameterset*const class_parameters)
{
  //call base class to merge parameters together
  CommunicationProtocol::Init(class_parameters);
  
  extract_parameter_value(_port,_params,"port");
  extract_parameter_value(_baudrate,_params,"baudrate");
  
  extract_parameter_map_cast(sbitmap,_stopbits,_params,"stopbits",false);
  extract_parameter_map_cast(flowmap,_flowcont,_params,"flowcontrol",false);
  
  extract_parameter_map_cast(paritymap,_parity,_params,"parity",false);
  
  extract_parameter_value(_timeout,_params,"timeout",false);
  
  extract_parameter_value(_bits,_params,"bits",false);
  
  
  
  
  
  boost::system::error_code ec;
  
  _sport.open(_port,ec);
  if(ec)
  {
    throw ProtocolError(std::string("can't open serial port: ") + ec.message().c_str());
  }
  
  
  _sport.set_option(serial_port_base::baud_rate(_baudrate));
  _sport.set_option(serial_port_base::parity(_parity));
  _sport.set_option(serial_port_base::stop_bits(_stopbits));
  _sport.set_option(serial_port_base::character_size(_bits));
  
  
  
};



std::string foxtrot::protocols::SerialPort::read(unsigned int len, unsigned* actlen)
{
  std::vector<unsigned char> out;
  out.resize(len);
  
  boost::system::error_code ec;
  
  if(actlen != nullptr)
  {
    *actlen = _sport.read_some(buffer(out),ec);
  }
  else
  {
    _sport.read_some(buffer(out),ec);
  }
  
  if(ec)
  {
    throw ProtocolError(std::string("error reading from serial port: ") + ec.message());
  }
  
  return std::string(out.begin(), out.end());
  
}

void foxtrot::protocols::SerialPort::write(const std::string& data)
{
  
  boost::system::error_code ec;
  _sport.write_some(buffer(data),ec);
  
  if(ec)
  {
   throw ProtocolError( std::string("error writing to serial port: ")  + ec.message()); 
  }
  
}


void foxtrot::protocols::SerialPort::flush()
{
  auto ret = tcflush(  _sport.native_handle(), TCIOFLUSH);
  if(ret < 0)
  {
    throw ProtocolError(std::string("failed to flush serial port: ") + strerror(ret));
  }

}


unsigned foxtrot::protocols::SerialPort::bytes_available()
{
  unsigned bytes_available;
  auto ret = ioctl(_sport.native_handle(), FIONREAD, &bytes_available);

  if(ret <0)
    {
      throw ProtocolError(std::string("ioctl failed: " ) + strerror(ret));
    };

  return bytes_available;

}


std::string foxtrot::protocols::SerialPort::read_until_endl(unsigned wait_ms, char endlchar)
{
  auto avail = bytes_available();
  auto ret = this->read(avail);
  
  decltype(ret.begin()) endlpos;


  while( (endlpos = std::find(ret.begin(),ret.end(),endlchar) ) == ret.end())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
      ret += read(bytes_available());

    };

  return ret;
  

}
