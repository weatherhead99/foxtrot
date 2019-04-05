#pragma once
#include <string>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/io_service.hpp>

#include <foxtrot/Logging.h>
#include <foxtrot/protocols/SerialProtocol.h>


using namespace boost::asio;

namespace foxtrot
{
  
  namespace protocols
  {
    
    
    class FOXTROT_SERVER_EXPORT SerialPort : public SerialProtocol
    {
    public:
    SerialPort(const parameterset*const instance_parameters);
    ~SerialPort();
    virtual void Init(const parameterset*const class_parameters) override;
    virtual std::string read(unsigned int len, unsigned* actlen= nullptr) override;
    virtual void write(const std::string& data) override;
    
    
    bool getDrain() const;
    void setDrain(bool drain);
    
    void flush();
    unsigned bytes_available();
    
    virtual std::string read_until_endl(char endlchar='\n') override;
    
    void setWait(unsigned wait_ms);
    unsigned getWait() const;
    
    
    private:
      foxtrot::Logging _lg;
        
      boost::asio::io_service _io_service;
      boost::asio::serial_port _sport;
      
      std::string _port;
      int _baudrate;
      
      serial_port_base::parity::type _parity = serial_port_base::parity::none;
      serial_port_base::flow_control::type _flowcont = serial_port_base::flow_control::none;
      serial_port_base::stop_bits::type _stopbits = serial_port_base::stop_bits::one;
      int _bits = 8;
      int _timeout = 5;//timeout in 10ths of a second
      
      int _wait_ms = 0;
      bool _drain = false;
      
    };
    
    
  }//namespace protocols
  
}//namespace foxtrot
