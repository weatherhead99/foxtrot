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
     //address range 1-24, default is 1
     short unsigned _address = 010; 
     
     string calculate_checksum(string::const_iterator start, string::const_iterator end);
     string calculate_checksum(const string& message);
     
     double interpret_u_expo_raw(const std::string& val);
     
     std::tuple<int,int,string> read_cmd_helper(int channel, TPG_parameter_no param)
     {
         auto ret = semantic_command<TPG_parameter_no>(_address + channel, param,
                                                       pfeiffer_action::read);
         auto interpret = interpret_response_telegram(ret);
         validate_response_telegram_parameters<TPG_parameter_no>(_address + channel, param, interpret);
         return interpret;
     }
     
     template<typename T>
     void write_cmd_helper(int channel, TPG_parameter_no param, T value)
     {
         auto st = str_from_number(static_cast<unsigned short>(value));
         auto ret = semantic_command<TPG_parameter_no>(_address + channel, param,
                                                       pfeiffer_action::describe);
         auto interpret = interpret_response_telegram(ret);
         validate_response_telegram_parameters<TPG_parameter_no>(_address + channel,
                                                                 param, interpret);
         
     }
     

   };
   
 }
  
  
}
