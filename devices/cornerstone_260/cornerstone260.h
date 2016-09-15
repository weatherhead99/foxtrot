#pragma once
#include "CmdDevice.h"
#include "SerialProtocol.h"

namespace foxtrot
{
  namespace devices
  {
    
    class cornerstone260 : public CmdDevice
    {
    public:
      cornerstone260(std::shared_ptr< SerialProtocol> proto);
      
    private:
      std::string cmd(const std::string& request) override;
      
      
      bool _cancelecho = true;
      std::shared_ptr<SerialProtocol> _serproto;
      
      
    };
    
    
    
  }//namespace devices
} //namespace foxtrot