#include <boost/asio.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/experimental/cancellation_condition.hpp>
#include <boost/system/detail/errc.hpp>
#include <boost/system/system_error.hpp>
#include <chrono>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <string.h>
#ifdef linux
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif


#include <foxtrot/StubError.h>
#include <foxtrot/ProtocolError.h>
#include <foxtrot/ProtocolTimeoutError.h>

#include <foxtrot/protocols/simpleTCP.h>
#include <foxtrot/protocols/ProtocolUtilities.h>

using namespace std::literals;
using namespace foxtrot::protocols;
using simpleTCP = foxtrot::protocols::simpleTCPLegacy;
using foxtrot::protocols::simpleTCPasio;
using namespace boost::asio::experimental::awaitable_operators;


foxtrot::protocols::simpleTCPBase::simpleTCPBase(
    const parameterset *const instance_parameters)
  : SerialProtocol(instance_parameters) {};    


simpleTCPLegacy::simpleTCPLegacy(const parameterset*const instance_parameters)
: simpleTCPBase(instance_parameters), _lg("simpleTCP")
{

}

simpleTCPLegacy::~simpleTCPLegacy()
{
  
#ifdef linux
  try
    {
    close();
    }
  catch(ProtocolError& err)
    {
      _lg.strm(sl::warning) << "caught exception closing socket FD, error was: "<< err.what();
    }
  catch(...)
    {
      _lg.strm(sl::warning) << "caught exception closing socket FD, unknown content";
    }
#else
	_lg.Error("destructor is a stub on windows");
#endif

}


void simpleTCPLegacy::Init(const parameterset* const class_parameters, bool open_immediate)
{
#ifndef linux
	throw StubError("simpleTCP::Init is a stub on windows!");
#else
	
  //call base class to merge parameterset
	CommunicationProtocol::Init(class_parameters, open_immediate);
  
  extract_parameter_value(_port,_params,"port");
  extract_parameter_value(_addr,_params,"addr");
  
  //open TCP socket
  _sockfd = socket(AF_INET, SOCK_STREAM, 0); //6 - TCP 
  if(_sockfd < -1)
  {
    auto perr = strerror(_sockfd);
    throw ProtocolError(std::string("couldn't open socket file descriptor. Error was:") + perr);  
  };
  
  //set linger option
  linger ling;
  ling.l_onoff = 1;
  ling.l_linger = 1;
  
  int err = setsockopt(_sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(linger));
  if(err <0 )
  {
    throw ProtocolError(std::string("error setting linger: " ) + gai_strerror(err));
  };

  if(open_immediate)
    this->open();
  else
    _lg.strm(sl::info) << "open_immediate was false, leaving socket unconnected for now";

  
#endif
}

void simpleTCPLegacy::open()
{
  addrinfo hints;  
  hints.ai_socktype = SOCK_STREAM; 
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = (AI_V4MAPPED | AI_ADDRCONFIG);
  hints.ai_protocol = IPPROTO_TCP;
  
  addrinfo* host;
  //unique_ptr to make sure freeaddrinfo gets called
  
  auto hosterr = getaddrinfo(_addr.c_str(), std::to_string(_port).c_str(),&hints,&host);
  std::unique_ptr<addrinfo, void(*)(addrinfo*)> addrp(host,freeaddrinfo);

  if(hosterr < 0)
    throw ProtocolError(std::string("host resolution error: " ) + gai_strerror(hosterr));
  
  _lg.Debug("ai_flags output: " + std::to_string(host->ai_flags));
  
  auto conerr = connect(_sockfd, host->ai_addr,host->ai_addrlen);
  if( conerr < 0)
    throw ProtocolError(std::string("couldn't connect to host. Error was: " ) + strerror(errno));

}

void simpleTCPLegacy::close()
{
  auto err = shutdown(_sockfd, SHUT_RDWR);
  if(err != 0)
    throw ProtocolError(std::string("couldn't close socket, Error was: " ) + strerror(err));
}






std::string simpleTCPLegacy::read(unsigned int len, unsigned* actlen)
{
#ifndef linux

	throw StubError("simpleTCP::read is a stub on windows!");
#else
	
  std::vector<unsigned char> out;
  out.resize(len);
  
  auto recv = ::read(_sockfd,out.data(),out.size());
  
  if(recv < 0)
  {
    throw ProtocolError(std::string("error reading from socket: " ) + gai_strerror(recv));
  };
  
  if(actlen != nullptr)
  {
    *actlen = recv;
  }
  
  return std::string(out.begin(),out.end());
#endif
  
}




void simpleTCPLegacy::write(const std::string& data)
{
#ifndef linux
	throw StubError("simpleTCP::write is a stub on windows!");
#else
  
  auto err = ::write(_sockfd,const_cast<char*>(data.data()),data.size());
  if(err <0)
  {
   throw ProtocolError(std::string("error writing to socket: ") + strerror(err)); 
    
  }

 #endif
  
}

std::string simpleTCPLegacy::read_until_endl(char endlchar)
{
  unsigned actlen;
  auto ret = this->read(_chunk_size, &actlen);
  
  decltype(ret.begin()) endlpos;
  while( (endlpos = std::find(ret.begin(), ret.end(), endlchar) ) == ret.end())
  {
    ret += read(_chunk_size);
  };

  //remove excess... NOTE BREAKING CHANGE
  return ret.substr(0, ret.find(endlchar, 0)+1);

}

unsigned int simpleTCPLegacy::bytes_available()
{
	#ifndef linux
	throw StubError("simpleTCP::bytes_available() is a stub on windows!");
	#else
	
	
  int count;
  auto ret = ioctl(_sockfd,FIONREAD,&count);
  if(ret <0)
  {
    throw ProtocolError(std::string("couldn't check bytes available:") + strerror(ret));
  }

  return count;
  
  #endif
}


unsigned int simpleTCPLegacy::getchunk_size()
{
  return _chunk_size;
}

void simpleTCPLegacy::setchunk_size(unsigned int chunk)
{
  _chunk_size = chunk;
}

struct foxtrot::protocols::detail::simpleTCPasioImpl
{
  simpleTCPasioImpl() : lg("simpleTCPasio") {};
  
  boost::asio::any_io_executor exec_;
  foxtrot::Logging lg;

  std::string addr;
  unsigned port;

  std::unique_ptr<boost::asio::io_context> ioptr = nullptr;
  opttimeout timeout;
  std::unique_ptr<boost::asio::ip::tcp::socket> sock = nullptr;
  
  bool use_internal_blocking_loop = false;
  
  void setup_executor(optional<boost::asio::any_io_executor> exec)
  {
    if(exec.has_value())
      {
       exec_ = *exec;
      }
    else
      {
	lg.strm(sl::warning) << "using default executor. Maybe breakage...";
	ioptr = std::make_unique<boost::asio::io_context>();
	exec_ = ioptr->get_executor();
	use_internal_blocking_loop = true;
      }
  }

  void ensure_exec_run()
  {
    if(use_internal_blocking_loop)
      {
	if(ioptr->stopped())
	  {
	    lg.strm(sl::debug) << "restarting internal io_context";
	    ioptr->restart();
	  }
	ioptr->run();

      }
  }

};

using namespace foxtrot::protocols;


simpleTCPasio::simpleTCPasio(const parameterset *const instance_parameters,
			     optional<boost::asio::any_io_executor> exec)
  : simpleTCPBase(instance_parameters)
{
  pimpl = std::make_unique<detail::simpleTCPasioImpl>();
  pimpl->setup_executor(exec);

  Init(nullptr, false);

}

simpleTCPasio::simpleTCPasio(const string* addr, optional<unsigned> port,
			     opttimeout timeout,
			     optional<boost::asio::any_io_executor> exec)
  : simpleTCPBase( std::make_pair("port", port.value_or(0u)),
		    std::make_pair("addr", (addr == nullptr) ? ""s : *addr))
{
  pimpl = std::make_unique<detail::simpleTCPasioImpl>();
  pimpl->setup_executor(exec);
  pimpl->timeout = timeout;

  if(addr != nullptr)
    pimpl->addr = *addr;
  if(port.has_value())
    pimpl->port = *port ;

}

simpleTCPasio::~simpleTCPasio() {}

void simpleTCPasio::Init(const parameterset *const class_parameters, bool open_immediate)
{
  foxtrot::CommunicationProtocol::Init(class_parameters, open_immediate);
  extract_parameter_value(pimpl->port, _params, "port");
  extract_parameter_value(pimpl->addr, _params, "addr");

  unsigned timeout_ms;
  extract_parameter_value(timeout_ms, _params, "timeout", false);
  pimpl->timeout = std::chrono::milliseconds(timeout_ms);

  //TODO: open  socket here!
  if(open_immediate)
    open();
  else
    {
      pimpl->lg.strm(sl::info) << "open_immediate was false. Deferring opening of socket";
    }
}

void simpleTCPasio::Init(const std::string* addr, std::optional<unsigned> port,
			 opttimeout timeout, bool open_immediate)
{
  if(port.has_value())
    pimpl->port = *port;
  if(addr != nullptr)
    pimpl->addr = *addr;

  pimpl->timeout = timeout;

  if(open_immediate)
    open();
  else
    {
      pimpl->lg.strm(sl::info) << "open_immediate was false. Deferring opening of socket";
    }
}

void simpleTCPasio::Init()
{
  extract_parameter_value(pimpl->port, _params, "port");
  extract_parameter_value(pimpl->addr, _params, "addr");

  open();
}

optional<bool> simpleTCPasio::try_connect() noexcept
{
  if(is_open().value_or(false))
    {
      pimpl->lg.strm(sl::debug) << "try_connect: socket already open";
      return true;
    }

  bool timeout_hasval = pimpl->timeout.has_value();
  bool success = false;
  //if there's no looser timeout specified, need a reasonably short one here
  if(not timeout_hasval)
    pimpl->timeout = std::chrono::milliseconds(500);
  try{
    open();
    close();
    success = true;
  }
  catch(...)
    {
      pimpl->lg.strm(sl::info) << "caught error whilst in try_connect. Swallowing...";
    }

  if(not timeout_hasval)
    pimpl->timeout = std::nullopt;

  return success;
}

optional<bool> simpleTCPasio::is_open()
{
  //NOTE: just currently returns is_open on the actual socket,
  //which isn't enough to check whether the connection is valid.
  //should e.g. do a 0 length read to check that.
  if(pimpl->sock == nullptr)
    return false;
  else
    return pimpl->sock->is_open();

}


struct timeout_manager
{
  foxtrot::Logging* lg = nullptr;
  opttimeout timeout = std::nullopt;
  boost::asio::any_io_executor exec;
  
  template <typename T, typename E>
  boost::asio::awaitable<std::optional<T>, E> wrap_coro_timeout(boost::asio::awaitable<T,E>&& coro_in)
  {
    if(timeout.has_value())
      {
	boost::asio::system_timer tmr(exec);
	tmr.expires_after(*timeout);

	auto res = co_await (tmr.async_wait(boost::asio::use_awaitable)  || std::move(coro_in));
	if(res.index() == 0)
	  {
	    //timer returned first, timeout;
	    if(lg)
	      lg->strm(sl::debug) << "asio operation timed out before completion...";
	    co_return std::nullopt;
	  }
	if(lg)
	  lg->strm(sl::debug) << "asio operation completed before timeout...";
	co_return std::get<1>(res);
      }
    else
      {
	if(lg)
	  lg->strm(sl::debug) << "no timeout set, timeout wrapper is a nop";
	co_return (co_await std::move(coro_in));
      }
    
  }
};




void simpleTCPasio::close() {
  if(pimpl->sock != nullptr)
    {
      pimpl->lg.strm(sl::debug) << "shutting down and closing open socket";
      try {
	pimpl->sock->shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);
	pimpl->sock->close();
	//reset unique_ptr
	pimpl->sock.reset(nullptr);
      }
      catch(boost::system::system_error& err)
	{
	  pimpl->lg.strm(sl::error) << "error closing the socket: " << err.what();
	  throw foxtrot::ProtocolError("error closing TCP socket, see log for details");
	} 
    }
}




void simpleTCPasio::open()
{
  boost::asio::ip::tcp::resolver resolver(pimpl->exec_);
  if(pimpl->sock == nullptr)
    {
      pimpl->lg.strm(sl::debug) << "new socket object";
      pimpl->sock = std::make_unique<boost::asio::ip::tcp::socket>(pimpl->exec_);
    }

    pimpl->lg.strm(sl::debug) << "resolving and opening socket";
    auto connfun = [&resolver, this] () ->boost::asio::awaitable<std::exception_ptr> {

      pimpl->lg.strm(sl::trace) << "dispatching async_resolve...";
      try{
	pimpl->lg.strm(sl::trace) << "port string: " << std::to_string(pimpl->port);
	pimpl->lg.strm(sl::trace) << "addr string: " << pimpl->addr;	

	auto resolve_results = co_await resolver.async_resolve(pimpl->addr, std::to_string(pimpl->port),
							   boost::asio::use_awaitable);
	pimpl->lg.strm(sl::debug) << "resolved: " << resolve_results.size() << " possible results";

	auto conn_coro =  boost::asio::async_connect(*(pimpl->sock),
						     resolve_results.begin(),
						     resolve_results.end(),
						     boost::asio::use_awaitable);


	timeout_manager tman;
	tman.timeout = pimpl->timeout;
	tman.exec = pimpl->exec_;
	tman.lg = &(pimpl->lg);
	auto res = co_await tman.wrap_coro_timeout(std::move(conn_coro));

	if(!res.has_value())
	  {
	    pimpl->lg.strm(sl::error) << "connection timed out!";
	    throw foxtrot::ProtocolTimeoutError("connection timed out!");
	  }
	else
	  {
	    pimpl->lg.strm(sl::debug) << "connected";
	  }
      
      }
      catch (...)
	{
	  co_return std::current_exception();
	}

      pimpl->lg.strm(sl::trace) << "returning no error condition";
      co_return nullptr;
  };

  //this function is synchronous
  auto res = boost::asio::co_spawn(pimpl->exec_, connfun(), boost::asio::use_future);
  pimpl->lg.strm(sl::trace) << "coroutine posted";
  pimpl->ensure_exec_run();

  pimpl->lg.strm(sl::trace) << "waiting on std::future...";  

  auto err = res.get();
  if(err)
    std::rethrow_exception(err);
  
}

std::string simpleTCPasio::read(unsigned len, unsigned *actlen) {
  if(pimpl->sock ==nullptr)
    {
      pimpl->lg.strm(sl::info) << "socket was closed, read request re-open";
      open();
    }

  std::vector<unsigned char> out;
  out.resize(len);
  
  auto readfun = [this, &out, &actlen] () -> boost::asio::awaitable<std::exception_ptr>
    {
      try{
	auto read_coro = boost::asio::async_read(*(pimpl->sock), boost::asio::buffer(out),
						 boost::asio::use_awaitable);

	timeout_manager tman;
	tman.lg = &(pimpl->lg);
	tman.timeout = pimpl->timeout;
	tman.exec = pimpl->exec_;

	auto res = co_await tman.wrap_coro_timeout(std::move(read_coro));

	if(!res.has_value())
	  {
	    throw foxtrot::ProtocolTimeoutError("read timed out!");
	  }
	else
	  {
	    pimpl->lg.strm(sl::debug) << "actual bytes transferred: " << *res;
	    *actlen = *res;
	  }
      }
      catch(...)
	{
	  co_return std::current_exception();
	}


      
      co_return nullptr;

    };


  auto res = boost::asio::co_spawn(pimpl->exec_, readfun(), boost::asio::use_future);
  pimpl->ensure_exec_run();
  auto err = res.get();
  if(err)
    std::rethrow_exception(err);


  string outstr;
  std::copy(out.begin(), out.end(), outstr.begin());
  return outstr;

}

void simpleTCPasio::write(const std::string &data) {

  if(pimpl->sock == nullptr)
    {
      pimpl->lg.strm(sl::info) << "socket was closed, write request re-open";
      open();
    }

  auto writefun = [this, &data] () -> boost::asio::awaitable<std::exception_ptr>
    {
      try {
      co_await boost::asio::async_write(*(pimpl->sock),
					boost::asio::buffer(data),
					boost::asio::use_awaitable);
      }
      catch(...)
	{
	  co_return std::current_exception();
	}
      
      co_return nullptr;
    };


  auto  res = boost::asio::co_spawn(pimpl->exec_, writefun(), boost::asio::use_future);
  pimpl->ensure_exec_run();
  pimpl->lg.strm(sl::trace) << "write() waiting for std::future";
  auto err = res.get();
  if(err)
    std::rethrow_exception(err);
  
}

std::string simpleTCPasio::read_until_endl(char endlchar) {
  if(pimpl->sock == nullptr)
    {
      pimpl->lg.strm(sl::info) << "socket was closed, write request re-open";
      open();
    }

  unsigned len_read;
  boost::asio::streambuf buf;
  auto ruefun = [this, &buf, endlchar, &len_read] () -> boost::asio::awaitable<std::exception_ptr>
    {
      try {
	auto ruecoro =  boost::asio::async_read_until(*(pimpl->sock),
						      buf, endlchar,
						      boost::asio::use_awaitable);

	timeout_manager tman;
	tman.exec = pimpl->exec_;
	tman.timeout = pimpl->timeout;
	tman.lg = &(pimpl->lg);

	auto res = co_await tman.wrap_coro_timeout(std::move(ruecoro));
	if(!res.has_value())
	  throw foxtrot::ProtocolTimeoutError("read_until_endl timed out");

	pimpl->lg.strm(sl::debug) << "read_until_endl actlen: " << *res;
	len_read = *res;

      }
      catch(...)
	{
	  co_return std::current_exception();
	}
      co_return nullptr;
    };

  auto res = boost::asio::co_spawn(pimpl->exec_, ruefun(), boost::asio::use_future);
  pimpl->ensure_exec_run();
  auto  err = res.get();
  if(err)
    std::rethrow_exception(err);

  std::istream iss(&buf);
  std::string out;
  std::getline(iss, out, endlchar);

  return out;  
 }

 unsigned simpleTCPasio::bytes_available() {
   if(pimpl->sock == nullptr)
     {
       pimpl->lg.strm(sl::warning) << "asked for bytes_available of a closed socket!";
       return 0u;
     }

   return pimpl->sock->available();

 }
