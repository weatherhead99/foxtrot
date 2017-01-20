#include "archon.h"

#include <algorithm>

#include "ProtocolUtilities.h"
#include "CommunicationProtocol.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "ProtocolError.h"

#define READ_SIZE 1024

foxtrot::devices::archon::archon(std::shared_ptr< foxtrot::protocols::simpleTCP > proto)
  : CmdDevice(std::static_pointer_cast<foxtrot::CommunicationProtocol>(proto)), _specproto(proto),
    _order(0)
{
  proto->Init(nullptr);
  
}


std::string foxtrot::devices::archon::cmd(const std::string& request)
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
  
  
  
  auto ret = _specproto->read_until_endl();
  
  
  //first characters should be "<xx"
  if(ret[0] != '<')
  {
    throw ProtocolError("invalid archon response!");
  };
  
  auto outret = std::stoul(ret.substr(1,2),nullptr,16);
  std::cout << "outret: " << outret << std::endl;
  
  if(outret != thisorder)
  {
    throw ProtocolError("mismatched order response");
  }
  
  //-3: 2 for the initial 2 chopped off, 1 for chopping off the endl at the end
  return ret.substr(3);

}

ssmap devices::archon::parse_parameter_response(const std::string& response)
{
  
  ssmap out;
  std::istringstream ss(response);
  std::string item;
  
  while(std::getline(ss, item, ' '))
  {
    auto eqpos = std::find(item.begin(), item.end(),'=');
    if(eqpos == item.end())
    {
      throw ProtocolError("malformed archon parameter response");
    };
    
    std::string key(item.begin(), eqpos);
    std::string val(eqpos + 1, item.end());
    
    out[key] = val;
    
  };
  return out;

}

ssmap foxtrot::devices::archon::getStatus()
{
    auto statmap = parse_parameter_response(cmd("SYSTEM"));
    return statmap ;
}
