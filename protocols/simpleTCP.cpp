#include "simpleTCP.h"

#include <boost/variant.hpp>
#include <vector>

#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>

#include <string.h>
#include <unistd.h>
#include "ProtocolError.h"

inline ssize_t c_read(int _fd, void* __buf, size_t nbytes)
{
    return read(_fd,__buf,nbytes);       
}

inline ssize_t c_write(int _fd, void* __buf, size_t nbytes)
{
  return write(_fd,__buf,nbytes);
  
};


simpleTCP::simpleTCP(const parameterset*const instance_parameters)
: SerialProtocol(instance_parameters)
{

}


simpleTCP::~simpleTCP()
{
  
  close(_sockfd);

}



void simpleTCP::Init(const parameterset* const class_parameters)
{
  //call base class to merge parameterset
  CommunicationProtocol::Init(class_parameters);
  
  try
  {
    _port = boost::get<decltype(_port)>(_params["port"]);
  }
  catch(boost::bad_get)
  {
    throw ProtocolError("invalid parameter for port!");
  }
  
  try
  {
    _addr = boost::get<decltype(_addr)>(_params["addr"]);
  }
  catch(boost::bad_get)
  {
    throw ProtocolError("invalid parameter for addr");
    
  };
  //TODO: logging here
  
  
  //open TCP socket
  _sockfd = socket(AF_INET, SOCK_STREAM, 0); //6 - TCP 
  if(_sockfd < -1)
  {
    auto perr = strerror(_sockfd);
    throw ProtocolError(std::string("couldn't open socket file descriptor. Error was:") + perr);  
  };
  
  //bind to server
  sockaddr_in servaddr;
  auto host = gethostbyname(_addr.c_str());
  
  if(host== nullptr)
  {
    throw ProtocolError("no such host!");
  };
  
  //FIXME: SEGFAULT HERE!!
  std::copy(host->h_addr, host->h_addr + host->h_length, reinterpret_cast<char*>( servaddr.sin_addr.s_addr));
  
  
  servaddr.sin_port = htons(_port);
  servaddr.sin_family = AF_INET;
  
  auto conerr = connect(_sockfd, reinterpret_cast<sockaddr*>(&servaddr),sizeof(servaddr));
  if( conerr < 0)
  {
    throw ProtocolError(std::string("couldn't connect to host. Error was: " ) + strerror(conerr));
  };
  
}


std::string simpleTCP::read(unsigned int len)
{
  std::vector<unsigned char> out;
  out.resize(len);
  
  auto recv = c_read(_sockfd,out.data(),out.size());
  
  if(recv < 0)
  {
    throw ProtocolError(std::string("error reading from socket: " ) + strerror(recv));
  };
  
  
  return std::string(out.begin(),out.end());
}




void simpleTCP::write(const std::string& data)
{
  
  auto err = c_write(_sockfd,const_cast<char*>(data.data()),data.size());
  if(err <0)
  {
   throw ProtocolError(std::string("error writing to socket: ") + strerror(err)); 
    
  }

}
