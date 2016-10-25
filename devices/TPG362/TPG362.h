#pragma once
#include "CmdDevice.h"
#include "SerialProtocol.h"
#include <memory>
#include <sstream>
#include <iomanip>

using std::string;

namespace foxtrot {
 namespace devices
 {
   
   enum class action
   {
    read,
    describe
   };
   
   enum class parameter_no
   {
     keylock = 8,
     degas = 040,
     sensEnable = 041,
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
   
   class TPG362 : public CmdDevice
   {
   public:
    TPG362(std::shared_ptr<SerialProtocol> proto);
    virtual std::string cmd(const std::string& request) override;
    template<typename T> std::string semantic_cmd(parameter_no p, action readwrite, const T& data);
    
    
    
   private:
     template<typename T> string str_from_number(T number, unsigned short width=3)
     {
       std::ostringstream oss;
       oss << std::setw(width) << std::setfill('0') << number;
       return oss.str();
     }
     
     std::shared_ptr<SerialProtocol> _serproto;
     
     bool _serialmode;
     //address range 1-24, default is 1
     short unsigned _address = 010; 
     
     string calculate_checksum(const string& message);
     
   };
   
  template <typename T> string TPG362::semantic_cmd(parameter_no p, action readwrite, const T& data)
  {
    auto dlen = data.size();
    std::ostringstream oss;
    //checksum and CR get put in by this->cmd 
    oss << str_from_number(_address,3) << str_from_number(readwrite,2) << str_from_number(dlen,2) << data;
    return cmd(oss.str());
  
  }

  template <> string TPG362::semantic_cmd<std::string>(parameter_no p, action readwrite, const string& data)
  {
    
    return semantic_cmd(p,readwrite,str_from_number(data));

  }

   
   
 }
  
  
}