#include "archon.h"

#include "CommunicationProtocol.h"

#include <sstream>
#include <iomanip>

#include "ProtocolError.h"


foxtrot::protocols::archon::archon(std::shared_ptr< foxtrot::protocols::simpleTCP > proto)
: Device(std::static_pointer_cast<foxtrot::CommunicationProtocol>(proto)), _specproto(proto)
{

}


std::string foxtrot::protocols::archon::archoncmd(const std::string& request)
{
  if(_order == 0xFE)
  {
    _order = 0;
  }
  
  std::ostringstream oss;
  oss << ">" << std::hex << ++_order << request << "\n";
  
  _specproto->write(oss.str());
  
  //maximum message size,"<xx:" +  1024 bytes of binary  = 1028
  auto ret = _specproto->read(1028);
  
  

}


