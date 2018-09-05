#pragma once
#include "CmdDevice.h"
#include "SerialProtocol.h"


namespace foxtrot
{
  namespace devices
  {
    
    class cornerstone260 : public CmdDevice
    {
      RTTR_ENABLE(CmdDevice)
    public:
      cornerstone260(std::shared_ptr< SerialProtocol> proto);
      
      virtual const std::string getDeviceTypeName() const;
      
      bool getShutterStatus();
      void setShutterStatus(bool status);
  
      void setWave(double wl_nm);
      double getWave();
      
      
      void setFilter(int fnum);
      int getFilter();

      void setGrating(int gr);
      int getGrating();
      
      void setGratingCalibration(int gr, int lines, double factor, double zero, double offset, std::string label);
      
    protected:
      std::string cmd(const std::string& request) override;
      std::string readecho(const std::string& request);
      
      bool _cancelecho = true;
      std::shared_ptr<SerialProtocol> _serproto;
      

            
    };
    
    
    
  }//namespace devices
} //namespace foxtrot
 