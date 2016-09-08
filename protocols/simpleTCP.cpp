#include "simpleTCP.h"

#include <boost/variant.hpp>
#include <vector>

#include <memory>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>

#include <string.h>
#include <unistd.h>
#include "ProtocolError.h"

#include <errno.h>


#include <iostream>

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
  
  //bind to server;
  std::unique_ptr<addrinfo,void(*)(addrinfo*)> hints(new addrinfo,freeaddrinfo);
  hints->ai_socktype = SOCK_STREAM; 
  hints->ai_family = AF_UNSPEC;
  hints->ai_flags = (AI_V4MAPPED | AI_ADDRCONFIG);
  hints->ai_protocol = IPPROTO_TCP;
  
  addrinfo* host;
  auto hosterr = getaddrinfo(_addr.c_str(),std::to_string(_port).c_str(),hints.get(),&host);
  
  //unique_ptr to make sure freeaddrinfo gets called
  std::unique_ptr<addrinfo, void(*)(addrinfo*)> addrp(host,freeaddrinfo);
  
  
  if(hosterr < 0)
  {
    throw ProtocolError(std::string("host resolution error: " ) + gai_strerror(hosterr));
  };
  
  
  std::cout << "ai_flags output: " << host->ai_flags << std::endl;
  
  auto conerr = connect(_sockfd, host->ai_addr,host->ai_addrlen);
  if( conerr < 0)
  {
    throw ProtocolError(std::string("couldn't connect to host. Error was: " ) + strerror(errno));
  };
  
}


std::string simpleTCP::read(unsigned int len)
{
  std::vector<unsigned char> out;
  out.resize(len);
  
  auto recv = c_read(_sockfd,out.data(),out.size());
  
  if(recv < 0)
  {
    throw ProtocolError(std::string("error reading from socket: " ) + gai_strerror(recv));
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
