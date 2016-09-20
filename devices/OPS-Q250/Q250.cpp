#include "Q250.h"


using namespace foxtrot;

foxtrot::devices::Q250::Q250(std::shared_ptr<protocols::scsiserial> proto)
: CmdDevice(proto), _scsiproto(proto)
{
  
  _scsiproto->Init(nullptr);
  

}

std::string foxtrot::devices::Q250::cmd(const std::string& request)
{
  
  
  
  

}
