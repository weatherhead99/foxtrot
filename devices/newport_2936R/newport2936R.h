#pragma once
#include "CmdDevice.h"
#include "SerialProtocol.h"
#include <memory>
#include "Logging.h"
#include <string>
using std::string;

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
    
    enum class powermodes : short unsigned
    {
      DC_cont =0,
      DC_sing = 1,
      Integrate = 2,
      PP_cont = 3,
      PP_sing = 4,
      pulse_cont = 5,
      pulse_single = 6,
      RMS = 7      
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
    
    double getTemperature();
    
    string getSerialNumber();
    
    powermodes getMode();
    void setMode(powermodes mode);
    
    void manualTriggerState(bool state);
    bool getTriggerState();
    
    void setTriggerStartMode(int mode);
    int getTriggerStartMode();
    
    void setTriggerEndMode(int mode);
    int getTriggerEndMode();
    
    void setExternalTriggerMode(int mode);
    int getExternalTriggerMode();
    
    void setTriggerEdge(int edge);
    int getTriggerEdge();
    
    std::string getcaldate();
    double getcaltemp();
    
    int getChannel();
    void setChannel(int chan);
    
    int getFilterMode();
    void setFilterMode(int mode);
    
    int getDigitalFilter();
    void setDigitalFilter(int value);
    
    int getAnalogFilter();
    void setAnalogFilter(int value);
    
    bool getBufferBehaviour();
    void setBufferBehaviour(bool mode);
    
    int getDataStoreCount();
    
    int getDataStoreSize();
    void setDataStoreSize(int size);
    
    void clearDataStore();
    
    bool getDataStoreEnable();
    void setDataStoreEnable(bool onoff);
    
    foxtrot::devices::powerunits getDataStoreUnits();
    void setDataStoreUnits(foxtrot::devices::powerunits units);
    
    int getDataStoreInterval();
    void setDataStoreInterval(int interval);
    
    double getDataStoreValue(int idx);
    std::vector<double> fetchDataStore(int begin, int end);
    std::vector<double> fetchDataStoreOldest(int n);
    std::vector<double> fetchDataStoreNewest(int n);
    
    private:
      foxtrot::Logging _lg;
      void strip_CRLF(std::string& buffer);
      
      std::shared_ptr<SerialProtocol> _proto;
      bool _usbmode;
    };
    
    

  }//namespace devices
  
}//namespace foxtrot

std::string convert_powerunit_to_string(foxtrot::devices::powerunits p, bool& ok);
