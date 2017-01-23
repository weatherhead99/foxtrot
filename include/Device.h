#pragma once

#include <memory>
#include <string>
#include <rttr/registration>



#define THIS_TYPE std::remove_reference<decltype(*this)>::type


namespace foxtrot
{

  class CommunicationProtocol;
  
  class Device
  {
    RTTR_ENABLE()
  public:
    Device(std::shared_ptr<CommunicationProtocol> proto);
    
    virtual const std::string getDeviceTypeName() const;
    
    
  protected:
    std::shared_ptr<CommunicationProtocol> _proto;
    
    
    
    
  };


}

