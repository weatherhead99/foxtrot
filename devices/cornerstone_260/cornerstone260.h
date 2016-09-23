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
      
      bool getShutterStatus();
      void setShutterStatus(bool status);
  
      void setWave(double wl_nm);
      double getWave();
      

    protected:
      std::string cmd(const std::string& request) override;
      std::string readecho(const std::string& request);
      
      bool _cancelecho = true;
      std::shared_ptr<SerialProtocol> _serproto;
      

            
    };
    
    
    
  }//namespace devices
} //namespace foxtrot
