#pragma once

#include "foxtrot_export.h"
#include <memory>
#include <string>
#include <rttr/registration>

#define THIS_TYPE std::remove_reference<decltype(*this)>::type


namespace foxtrot
{
    
 enum class FOXTROT_EXPORT CapabilityMeta
 {
  STREAMINGDATA 
     
 };

  class CommunicationProtocol;
  
  class FOXTROT_EXPORT Device
  {
    RTTR_ENABLE()
  public:
    Device(std::shared_ptr<CommunicationProtocol> proto, const std::string& comment = "");
    
    virtual const std::string getDeviceTypeName() const;
    const std::string getDeviceComment() const;
    void setDeviceComment(const std::string& comment);
    
  protected:
    std::shared_ptr<CommunicationProtocol> _proto;
    std::string _devcomment;
    
    
    
  };


}
