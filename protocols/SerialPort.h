#pragma once
#include "SerialProtocol.h"
#include <string>

#include <boost/asio/serial_port.hpp>

using namespace boost::asio;

namespace foxtrot
{
  
  namespace protocols
  {
    
    
    class SerialPort : public SerialProtocol
    {
    public:
    SerialPort(const parameterset*const instance_parameters);
    ~SerialPort();
    virtual void Init(const parameterset*const class_parameters) override;
    virtual std::string read(unsigned int len, unsigned* actlen= nullptr) override;
    virtual void write(const std::string& data) override;
    
    void flush();
    unsigned bytes_available();
    
    std::string read_until_endl( unsigned wait_ms = 0, char endlchar='\n');
    
    private:
      boost::asio::io_service _io_service;
      boost::asio::serial_port _sport;
      
      std::string _port;
      unsigned _baudrate;
      
      serial_port_base::parity::type _parity = serial_port_base::parity::none;
      serial_port_base::flow_control::type _flowcont = serial_port_base::flow_control::none;
      serial_port_base::stop_bits::type _stopbits = serial_port_base::stop_bits::one;
      unsigned _bits = 8;
      unsigned _timeout = 5;//timeout in 10ths of a second
    };
    
    
  }//namespace protocols
  
}//namespace foxtrot
