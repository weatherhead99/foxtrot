#include "SerialPort.h"
#include "ProtocolError.h"
#ifdef linux
#include <termios.h>
#else
#include <Windows.h>
#endif

#include "ProtocolUtilities.h"
#include "StubError.h"

#include <fcntl.h>
#include <string.h>
#include <vector>

#include <iostream>
#include <map>

#include <chrono>
#include <thread>

#include <algorithm>

#include <boost/asio/write.hpp>

using namespace boost::asio;
using boost::asio::serial_port_base;


const std::map<int, serial_port_base::stop_bits::type> sbitmap 
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
: SerialProtocol(instance_parameters), _sport(_io_service), _lg("SerialPort")
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
  
  auto num_written = boost::asio::write(_sport,buffer(data),ec);
//   _sport.write_some(buffer(data),ec);

    if(_drain)
    {
  
    #ifdef linux
    //drain serial port
      _lg.strm(sl::trace) << "draining serial port";
    auto ret = tcdrain( _sport.native_handle() );
    if(ret)
    {
        _lg.strm(sl::error) << "serial port drain error: " << std::to_string(errno);
        
    }
    #else
    
    throw std::logic_error("draining serial port not implemented on Windows");

    #endif
    }
  
  
  if(ec)
  {
   throw ProtocolError( std::string("error writing to serial port: ")  + ec.message()); 
  }
  
  if(num_written != data.size())
  {
    throw ProtocolError("some bytes not written!");
  }
  
  
}

bool foxtrot::protocols::SerialPort::getDrain() const
{
    return _drain;
}

void foxtrot::protocols::SerialPort::setDrain(bool drain)
{
    _drain = drain;
}


void foxtrot::protocols::SerialPort::flush()
{
#ifdef linux
	
  auto ret = tcflush(  _sport.native_handle(), TCIOFLUSH);
  if(ret < 0)
  {
    throw ProtocolError(std::string("failed to flush serial port: ") + strerror(ret));
  }
#else	
    auto ret = PurgeComm(_sport.native_handle(), PURGE_RXCLEAR);
    if(ret)
    {
        throw ProtocolError(std::string("failed to clear rx buffer: " ) + std::to_string(GetLastError()));
    }
    ret = PurgeComm(_sport.native_handle(), PURGE_TXCLEAR);
    if(ret)
    {
        throw ProtocolError(std::string("failed to clear tx buffer: " ) + std::to_string(GetLastError()));
    }
#endif
}


unsigned foxtrot::protocols::SerialPort::bytes_available()
{
#ifdef linux
  unsigned bytes_available;
  auto ret = ioctl(_sport.native_handle(), FIONREAD, &bytes_available);

  if(ret <0)
    {
      throw ProtocolError(std::string("ioctl failed: " ) + strerror(ret));
    };

  return bytes_available;
#else
    unsigned bytes_available;
    DWORD Errors;
    COMSTAT Stat;
    auto ret = ClearCommError(_sport.native_handle(), &Errors, &Stat);
    if(!ret)
    {
        throw ProtocolError(std::string("windows API error: ") + std::to_string(GetLastError()));
    }
    
    return Stat.cbInQue;
    
#endif
  
}


std::string foxtrot::protocols::SerialPort::read_until_endl(char endlchar)
{
  auto avail = bytes_available();
  auto ret = this->read(avail);
  
  decltype(ret.begin()) endlpos;


  while( (endlpos = std::find(ret.begin(),ret.end(),endlchar) ) == ret.end())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(_wait_ms));
      ret += read(bytes_available());

    };

  return ret;
  

}

unsigned int foxtrot::protocols::SerialPort::getWait() const
{
  return _wait_ms;

}

void foxtrot::protocols::SerialPort::setWait(unsigned int wait_ms)
{
  _wait_ms = wait_ms;

}

