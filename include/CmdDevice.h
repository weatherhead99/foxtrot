#pragma once
#include "Device.h"
#include <string>

namespace foxtrot
{
  
  class FOXTROT_CORE_EXPORT CmdDevice : public Device 
  {
	RTTR_ENABLE(Device);
  public:
    CmdDevice(std::shared_ptr< CommunicationProtocol > proto);
    CmdDevice(std::shared_ptr< CommunicationProtocol > proto, const std::string& comment);
    
  private:
    virtual std::string cmd(const std::string& request) = 0;
    
  };
  
  
}//namespace foxtrot
