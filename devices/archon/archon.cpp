#include "archon.h"

#include "CommunicationProtocol.h"

#include <sstream>
#include <iomanip>
#include <iostream>

#include "ProtocolError.h"


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
  
  std::ostringstream oss;
  oss << ">" << std::hex << std::setw(2)<< std::setfill('0') << ++_order << request << "\n";
  
  std::cout << "request is: " << oss.str();

  _specproto->write(oss.str());
  
  std::cout << "command written, waiting for reply..." << std::endl;
  //maximum message size,"<xx:" +  1024 bytes of binary  = 1028
  auto ret = _specproto->read(2048);
  
  //first characters should be "<xx"
  if(ret[0] != '<')
  {
    throw ProtocolError("invalid archon response!");
  };
  
  auto outret = std::stoul(ret.substr(1,1),nullptr,16);
  
  std::cout << "parsed order: " << outret << std::endl;
  
  return ret.substr(2,ret.size()-2);

}


