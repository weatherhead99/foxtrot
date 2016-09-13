#include "archon.h"

#include <algorithm>

#include "CommunicationProtocol.h"

#include <sstream>
#include <iomanip>
#include <iostream>

#include "ProtocolError.h"

#define READ_SIZE 1024

foxtrot::devices::archon::archon(std::shared_ptr< foxtrot::protocols::simpleTCP > proto)
  : Device(std::static_pointer_cast<foxtrot::CommunicationProtocol>(proto)), _specproto(proto),
    _order(0)
{
  proto->Init(nullptr);
  
}


std::string foxtrot::devices::archon::archoncmd(const std::string& request)
{
  if(_order == 0xFE)
  {
    _order = 0;
  }
  auto thisorder = _order;
  
  std::ostringstream oss;
  oss << ">" << std::hex << std::setw(2)<< std::setfill('0') << _order++ << request << "\n";
  
  std::cout << "request is: " << oss.str();

  _specproto->write(oss.str());
  
  std::cout << "command written, waiting for reply..." << std::endl;
  //maximum message size,"<xx:" +  1024 bytes of binary  = 1028
  auto ret = _specproto->read(READ_SIZE);
  
  //read until we have an endl
  while( std::find(ret.begin(), ret.end(),'\n')  == ret.end() )
  {
    std::cout << "need to read more.." << std::endl;
    //not a complete response
    ret += _specproto->read(READ_SIZE);
    
  }
  
  //chop off everything beyond the endl
  auto endlpos = std::find(ret.begin(), ret.end(), '\n');
  ret = std::string(ret.begin(), endlpos - 1);
  
  
  //first characters should be "<xx"
  if(ret[0] != '<')
  {
    throw ProtocolError("invalid archon response!");
  };
  
  auto outret = std::stoul(ret.substr(1,1),nullptr,16);
  if(outret != thisorder)
  {
    throw ProtocolError("mismatched order response");
  }
  
  //-3: 2 for the initial 2 chopped off, 1 for chopping off the endl at the end
  return ret.substr(3);

}


