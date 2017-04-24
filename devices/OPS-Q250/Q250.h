#pragma once

#include "CmdDevice.h"
#include "scsiserial.h"

#include <rttr/registration>


namespace foxtrot
{
  namespace devices
  {
    class Q250 : public CmdDevice
    {
      
      RTTR_ENABLE(CmdDevice)
      
    public:
    Q250(std::shared_ptr<protocols::scsiserial> proto);
    
    virtual std::string cmd(const std::string& request);
    virtual const std::string getDeviceTypeName() const;
    
    void start();
    void stop();
    
    
    double getAmps();
    double getAmpLimit();
    double getAmpPreset();
    
    double getVolts();
    
    int getWatts();
    
    
    private:
      std::shared_ptr<protocols::scsiserial> _scsiproto;
      
    };
  } //namespace devices
} //namespace foxtrot