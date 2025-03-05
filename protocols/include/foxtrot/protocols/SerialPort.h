#pragma once
#include <boost/asio/io_context.hpp>
#include <chrono>
#include <string>
#include <boost/asio/serial_port.hpp>

#include <foxtrot/Logging.h>
#include <foxtrot/protocols/SerialProtocol.h>
#include <future>
#include <memory>

using namespace boost::asio;

namespace foxtrot
{

namespace protocols
{

  using std::shared_ptr;
  using std::unique_ptr;
  
class  SerialPort : public SerialProtocol
{
public:
  [[deprecated("opening serial port without an io_context")]] SerialPort ( const parameterset*const instance_parameters );
    SerialPort ( const parameterset*const instance_parameters,
		 shared_ptr<io_context> ctxt);
    ~SerialPort();
    virtual void Init ( const parameterset*const class_parameters ) override;

    //this read is the one we just deprecated!
    virtual std::string read ( unsigned int len, unsigned* actlen= nullptr) override;
    virtual std::string read_definite(unsigned int len, opttimeout wait=nullopt) override;

  virtual std::string read_all(unsigned short read_at_least, std::chrono::milliseconds timeout
			       );

    virtual void write ( const std::string& data ) override;


    bool getDrain() const;
    void setDrain ( bool drain );

    void flush();
    unsigned bytes_available();

    virtual std::string read_until_endl ( char endlchar='\n' ) override;

    void setWait ( unsigned wait_ms );
    unsigned getWait() const;


    void reconnect(opttimeout wait_before_reconnect); 
  
    std::chrono::milliseconds calc_minimum_transfer_time(std::size_t len);
    
private:

  std::string read_until_endl_asio_impl(char endlchar='\n', opttimeout wait=nullopt);
  std::string read_until_endl_poll_impl(char endlchar='\n');
  
    foxtrot::Logging _lg;

    shared_ptr<io_context> _io_service;
    unique_ptr<serial_port> _sport;
    //    boost::asio::serial_port _sport;

    std::string _port;
    int _baudrate;

    serial_port_base::parity::type _parity = serial_port_base::parity::none;
    serial_port_base::flow_control::type _flowcont = serial_port_base::flow_control::none;
    serial_port_base::stop_bits::type _stopbits = serial_port_base::stop_bits::one;
    int _bits = 8;
    int _timeout = 5;//timeout in 10ths of a second

    unsigned _wait_ms = 0;
    bool _drain = false;

};


}//namespace protocols

}//namespace foxtrot
