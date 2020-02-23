#pragma once
#include <memory>
#include <sstream>
#include <iomanip>
#include <tuple>

#include <foxtrot/Logging.h>

#include <foxtrot/server/CmdDevice.h>
#include <foxtrot/protocols/SerialProtocol.h>

#include "Pfeiffer.h"


using std::string;

namespace foxtrot {
 namespace devices
 {
   
   enum class TPG_parameter_no : short unsigned
   {
     keylock = 8,
     degas = 40,
     sensEnable = 41,
     cfgRelay1 = 45,
     cfgRelay2 = 46,
     cfgRelay3 = 47,
     cfgRelay4 = 48,
     ErrorCode = 303,
     FWVersion = 312,
     OperatHours = 314,
     DeviceName = 349,
     HWVersion = 354,
     SWOnThrs = 730,
     SwOffThrs = 732,
     Pressure = 740,
     PressCorr = 742,
     RS485Addr = 797,
   };
   
   class TPG362 : public PfeifferDevice
   {
    RTTR_ENABLE()
   public:
    TPG362(std::shared_ptr<SerialProtocol> proto);
    
    double getPressure(short unsigned channel=1);
    std::string getDeviceName(short unsigned channel=1);
    
    bool getGaugeOnOff(short unsigned channel=1);
    void setGaugeOnOff(short unsigned channel,bool onoff);
    
    bool getDegass(short unsigned channel = 1);
    void setDegass(short unsigned channel, bool onoff);
    
    virtual const string getDeviceTypeName() const override;
   
    
   private:
     //address range 1-24, default is 1, shift left by one digit ( last digit is for gauge subaddr)
     short unsigned _address = 10; 

   };
   
 }
  
  
}
