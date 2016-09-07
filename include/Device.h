#pragma once

#include <memory>

namespace foxtrot
{

  class CommunicationProtocol;
  
  class Device
  {
  public:
    Device(std::shared_ptr<CommunicationProtocol> proto);
    
    
  private:
    std::shared_ptr<CommunicationProtocol> _proto;
    
    
  };


}