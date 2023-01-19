#pragma once

#include <foxtrot/CmdDevice.h>
#include <foxtrot/protocols/scsiserial.h>

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
    int getLampHours();
    double getVolts();
    
    int getWatts();
    
    void setAmpPreset(double amps);
    
    
    
    private:
      std::shared_ptr<protocols::scsiserial> _scsiproto;
      
    };
  } //namespace devices
} //namespace foxtrot
