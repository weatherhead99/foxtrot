#pragma once
#include "CmdDevice.h"
#include "SerialProtocol.h"
#include <memory>

namespace foxtrot
{
  namespace devices
  {
    class newport2936R : public CmdDevice
    {
    public:
    newport2936R(std::shared_ptr< SerialProtocol> proto);
    virtual std::string cmd(const std::string& request);
    
    
    
        
    private:
      std::shared_ptr<SerialProtocol> _proto;
    };
    
  }//namespace devices
  
}//namespace foxtrot