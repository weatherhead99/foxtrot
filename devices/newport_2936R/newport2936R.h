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
    
    void setLambda(int l);
    int getLambda();
    
    double getPower();
        
    double getResponsivity();
    double getArea();
    private:
      
      void strip_CRLF(std::string& buffer);
      
      std::shared_ptr<SerialProtocol> _proto;
      bool _usbmode;
    };
    
  }//namespace devices
  
}//namespace foxtrot