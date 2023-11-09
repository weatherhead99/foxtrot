#pragma once
#include <foxtrot/CmdDevice.h>
#include <foxtrot/Logging.h>

namespace foxtrot
{
  namespace protocols
  {
    class SerialPort;
  }

  
  namespace devices
  {
    
    class cornerstone260 : public CmdDevice
    {
      RTTR_ENABLE(CmdDevice)

    public:
      cornerstone260(std::shared_ptr<foxtrot::protocols::SerialPort> proto);
      
      const std::string getDeviceTypeName() const override;
      
      bool getShutterStatus();
      void setShutterStatus(bool status);
  
      void setWave(double wl_nm);
      double getWave();
      
      
      void setFilter(int fnum);
      int getFilter();

      void setGrating(int gr);
      int getGrating();
      
      void setGratingCalibration(int gr, int lines, double factor, double zero, double offset, std::string label);

      void abort();
      std::string lasterror();
      bool errstatus();

      std::string info();
      
    protected:
      std::string cmd(const std::string& request) override;
      void cmd_no_response(const std::string& request);
      std::string readecho(const std::string& request);

      void err_check();
      
      
      bool _cancelecho = true;
    private:
      foxtrot::Logging _lg;

            
    };
    
    
    
  }//namespace devices
} //namespace foxtrot
 
