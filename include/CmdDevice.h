#pragma once
#include "Device.h"
#include <string>

namespace foxtrot
{
  
  class CmdDevice : public Device 
  {
  public:
    CmdDevice(std::shared_ptr< CommunicationProtocol > proto);
    CmdDevice(std::shared_ptr< CommunicationProtocol > proto, const std::string& comment);
    
  private:
    virtual std::string cmd(const std::string& request) = 0;
    
  };
  
  
}//namespace foxtrot
