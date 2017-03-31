#pragma once
#include "CmdDevice.h"
#include "SerialProtocol.h"
#include <memory>
#include "Logging.h"

namespace foxtrot
{
  namespace devices
  {
    
    enum class powerunits : short unsigned
    {
      
     Amps = 0,
     Volts = 1,
     Watts = 2,
     Watts_cm2 = 3,
     Joules = 4,
     Joules_cm2 = 5
    };
    
    class newport2936R : public CmdDevice
    {
      RTTR_ENABLE()
    public:
    newport2936R(std::shared_ptr< SerialProtocol> proto);
    virtual const std::string getDeviceTypeName() const override;
    virtual std::string cmd(const std::string& request);
    
    void setLambda(int l);
    int getLambda();
    
    double getPower();
        
    double getResponsivity();
    double getArea();
    
    powerunits getUnits();
    void setUnits(powerunits unit);
    
    private:
      foxtrot::Logging _lg;
      void strip_CRLF(std::string& buffer);
      
      std::shared_ptr<SerialProtocol> _proto;
      bool _usbmode;
    };
    
    
    
  }//namespace devices
  
}//namespace foxtrot

std::string convert_powerunit_to_string(foxtrot::devices::powerunits p, bool& ok);