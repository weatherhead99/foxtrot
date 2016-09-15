#pragma once
#include "Device.h"
#include "SerialProtocol.h"

namespace foxtrot
{
  namespace devices
  {
    
    class cornerstone260 : public Device
    {
    public:
      cornerstone260(std::shared_ptr< SerialProtocol> proto);
      
    private:
      std::string cornerstonecmd(const std::string& request);
      
      
      bool _cancelecho = true;
      std::shared_ptr<SerialProtocol> _serproto;
      
      
    };
    
    
    
  }//namespace devices
} //namespace foxtrot