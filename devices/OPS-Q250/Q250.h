#pragma once

#include "CmdDevice.h"
#include "scsiserial.h"


namespace foxtrot
{
  namespace devices
  {
    class Q250 : public CmdDevice
    {
    public:
    Q250(std::shared_ptr<protocols::scsiserial> proto);
    
    virtual std::string cmd(const std::string& request);
    
    void start();
    void stop();
    
    
    double getAmps();
    double getVolts();
    double getWatts();
    
    
    private:
      std::shared_ptr<protocols::scsiserial> _scsiproto;
      
    };
  } //namespace devices
} //namespace foxtrot