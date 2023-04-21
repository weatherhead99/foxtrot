#include <boost/asio/detail/io_control.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/serial_port_base.hpp>
#include <boost/system/error_code.hpp>
#include <ios>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <map>
#include <chrono>
#include <thread>
#include <cmath>
#include <algorithm>
#ifdef __linux__
#include <termios.h>
#else
#include <Windows.h>
#endif
#include <fcntl.h>
#include <string.h>

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/completion_condition.hpp>
#include <boost/asio/read_until.hpp>

#include <foxtrot/ProtocolError.h>
#include <foxtrot/ProtocolTimeoutError.h>
#include <foxtrot/StubError.h>

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/protocols/ProtocolUtilities.h>


using namespace boost::asio;
using boost::asio::serial_port;
using boost::asio::serial_port_base;
using boost::asio::steady_timer;


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


foxtrot::protocols::SerialPort::SerialPort(  const foxtrot::parameterset *const instance_parameters)
  : SerialProtocol(instance_parameters), 
      _lg("SerialPort")
{
  _io_service = std::make_shared<io_context>();
  _sport = std::make_unique<serial_port>(*_io_service);
  
}

foxtrot::protocols::SerialPort::SerialPort(
    const foxtrot::parameterset *const instance_parameters,
    shared_ptr<boost::asio::io_context> ctxt)
  : SerialProtocol(instance_parameters), _lg("SerialPort"), _io_service(ctxt)
{
  if(!ctxt)
    throw foxtrot::ProtocolError("passed nullptr io_context to serialport!");
  _sport = std::make_unique<serial_port>(*_io_service);
  
};
					


foxtrot::protocols::SerialPort::~SerialPort()
{
  //TODO: error checking
  boost::system::error_code ec;
  _sport->close(ec);
  
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
  
  _sport->open(_port,ec);
  if(ec)
  {
    throw ProtocolError(std::string("can't open serial port: ") + ec.message().c_str());
  }
  
  
  _sport->set_option(serial_port_base::baud_rate(_baudrate));
  _sport->set_option(serial_port_base::parity(_parity));
  _sport->set_option(serial_port_base::stop_bits(_stopbits));
  _sport->set_option(serial_port_base::character_size(_bits));
  
  
  
};



std::string foxtrot::protocols::SerialPort::read(unsigned int len, unsigned* actlen)
{
  std::vector<unsigned char> out;
  out.resize(len);

  boost::system::error_code ec;

  auto actread = _sport->read_some(buffer(out), ec);

  if(actlen != nullptr)
    *actlen = actread;
    
  if(ec)
    throw ProtocolError(std::string("error reading from serial port: ") + ec.message());
  
  return std::string(out.begin(), out.end());
}

std::string foxtrot::protocols::SerialPort::read_until_endl_poll_impl(char endlchar)
{
  
  auto avail = bytes_available();
  auto ret = this->read(avail);
  
  decltype(ret.begin()) endlpos;


  while( (endlpos = std::find(ret.begin(),ret.end(),endlchar) ) == ret.end())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(_wait_ms));
      avail = bytes_available();
      if(avail == 0)
          throw foxtrot::ProtocolError("ran out of bytes to read in serial port! Perhaps wait_ms is not configured properly or this command does not return a response");
      ret += read(bytes_available());

    };

  return ret;
}

std::string foxtrot::protocols::SerialPort::read_until_endl_asio_impl(char endlchar, opttimeout wait)
{
  std::string strbuf;
  auto buf = boost::asio::dynamic_buffer(strbuf);
  boost::system::error_code ec;
  bool done = false;

  if(!wait.has_value())
    {
      _lg.strm(sl::trace) << "no timeout value, doing boost::asio::read_until";
      boost::asio::read_until(*_sport, buf, endlchar, ec);
    }
  else
    {
      _lg.strm(sl::trace) << "timeout value, doing boost::async_read_until";
      boost::asio::async_read_until(*_sport, buf,
				    endlchar,
				    [&ec, &done](const boost::system::error_code ec2, std::size_t bytes_transferred){ ec = ec2; done = true;});

      auto n_run = _io_service->run_for(*wait);
      _io_service->restart();
      
      if(!done)
	throw foxtrot::ProtocolTimeoutError("serial port read timed out...");
      
    }

  if(ec)
    throw ProtocolError(std::string("error reading from serial port: " ) +ec.message());
 
      
  return strbuf;
  

}



std::string foxtrot::protocols::SerialPort::read_definite(unsigned int len, opttimeout wait)
{
  std::vector<unsigned char> out;
  out.resize(len);

  boost::system::error_code ec;
  bool done = false;
  std::size_t actlen;
  auto cond = boost::asio::transfer_exactly(len);
  
  if(!wait.has_value())
    {
      _lg.strm(sl::trace) << "no timeout value, doing boost::asio::read";
      actlen = boost::asio::read(*_sport, buffer(out),
				 cond,
				 ec);
    }
  else
    {
      _lg.strm(sl::trace) << "timeout value, doing boost::async_read";
      boost::asio::async_read(*_sport, buffer(out),
			      cond,
			      [&ec, &done, &actlen](const boost::system::error_code ec2, std::size_t bytes_transferred) { ec = ec2; done=true; actlen = bytes_transferred; });

      _lg.strm(sl::trace) << "starting run_one_for on io_loop" ;
      auto n_run = _io_service->run_for(*wait);
      _lg.strm(sl::trace) << "run_one_for returned";
      _lg.strm(sl::trace) << "number of posts run: " << n_run;
      _lg.strm(sl::trace) << "io_service stopped? " << (int) _io_service->stopped();
      _io_service->restart();

      if(!done)
	  throw foxtrot::ProtocolTimeoutError("serial port read timed out...");
    }

  if(actlen != len)
    {
      _lg.strm(sl::error) << "expected bytes transferred: " << len;
      _lg.strm(sl::error) << "actual bytes transferred: " << actlen;
      throw std::logic_error(std::string("actlen bytes transferred not equal to requested bytes transferred, should never happen!"));
    }
  if(ec)
    throw ProtocolError(std::string("error reading from serial port: " ) +ec.message());
 

  return std::string(out.begin(), out.end());
}





void foxtrot::protocols::SerialPort::write(const std::string& data)
{
  
  boost::system::error_code ec;
  
  auto num_written = boost::asio::write(*_sport,buffer(data),ec);
//   _sport.write_some(buffer(data),ec);

    if(_drain)
    {
  
    #ifdef __linux__
    //drain serial port
      _lg.strm(sl::trace) << "draining serial port";
    auto ret = tcdrain( _sport->native_handle() );
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
#ifdef __linux__
	
  auto ret = tcflush(  _sport->native_handle(), TCIOFLUSH);
  if(ret < 0)
    throw ProtocolError(std::string("failed to flush serial port: ") + strerror(ret));

#else
    auto ret = PurgeComm(_sport->native_handle(), PURGE_RXCLEAR);
    if(ret)
      throw ProtocolError(std::string("failed to clear rx buffer: " ) + std::to_string(GetLastError()));
    
    ret = PurgeComm(_sport->native_handle(), PURGE_TXCLEAR);
    if(ret)
      throw ProtocolError(std::string("failed to clear tx buffer: " ) + std::to_string(GetLastError()));

#endif
}


unsigned foxtrot::protocols::SerialPort::bytes_available()
{
#ifdef __linux__
  unsigned bytes_available;
  auto ret = ioctl(_sport->native_handle(), FIONREAD, &bytes_available);

  if(ret <0)
    {
      throw ProtocolError(std::string("ioctl failed: " ) + strerror(ret));
    };

  return bytes_available;
#else
    unsigned bytes_available;
    DWORD Errors;
    COMSTAT Stat;
    auto ret = ClearCommError(_sport->native_handle(), &Errors, &Stat);
    if(!ret)
    {
        throw ProtocolError(std::string("windows API error: ") + std::to_string(GetLastError()));
    }
    
    return Stat.cbInQue;
    
#endif
  
}


std::string foxtrot::protocols::SerialPort::read_until_endl(char endlchar)
{
  return read_until_endl_poll_impl(endlchar);

}

unsigned int foxtrot::protocols::SerialPort::getWait() const
{
  return _wait_ms;

}

void foxtrot::protocols::SerialPort::setWait(unsigned int wait_ms)
{
  _wait_ms = wait_ms;

}


std::chrono::milliseconds foxtrot::protocols::SerialPort::calc_minimum_transfer_time(std::size_t len)
{
  serial_port_base::baud_rate brate;
  _sport->get_option(brate);

  _lg.strm(sl::trace) << "brate_value: " << brate.value();
  
  double time_ms = 1./brate.value() * len * 1000;
  _lg.strm(sl::trace) << "time_ms: " << time_ms;
  
  int ceil_ms = std::ceil(time_ms);
  _lg.strm(sl::trace) << "ceil_ms: " << ceil_ms;

  return std::chrono::milliseconds(ceil_ms);

}

