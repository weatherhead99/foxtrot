#pragma once
#include <foxtrot/Device.h>
#include <string>

using foxtrot::Device;

namespace foxtrot
{
  
  class  CmdDevice : public Device 
  {
	RTTR_ENABLE(Device);
  public:
    CmdDevice(std::shared_ptr< CommunicationProtocol > proto);
    CmdDevice(std::shared_ptr< CommunicationProtocol > proto, const std::string& comment);
    
  private:
    virtual std::string cmd(const std::string& request) = 0;
    
  };
  
  
}//namespace foxtrot
