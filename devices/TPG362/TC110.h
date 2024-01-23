#pragma once


#include "Pfeiffer.h"

namespace foxtrot {
    namespace devices {
        enum class TC110_parameter_no : short unsigned
        {
            Heating = 1,        Standby = 2,        RUTimeCtrl = 4,
            ErrorAckn = 9,      PumpgStatn = 10,    EnableVent = 12,
            CfgSpdSwPt = 17,    CfgDO2 = 19,        MotorPump = 23,
            CfgDO1 = 24,        OpModeBKP = 25,     SpdSetMode = 26,
            GasMode = 27,       VentMode = 30,      CfgAccA1 = 35,
            CfgAccB1 = 36,      CfgAccA2 = 37,      CfgAccB2 = 38,
            SealingGas = 50,    CfgAO1 = 55,        CtrlViaInt = 60,
            IntSelLckd = 61,    CfgDI1 = 62,        CfgDI2 = 63,
            RemotePrio = 300,   SpdSwPtAtt = 302,   ErrorCode = 303,
            OvTempElec = 304,   OvTempPump = 305,   SetSpdAtt = 306,
            PumpAccel = 307,    SetRotSpdHz = 308,  ActualSpdHz = 309,
            DrvCurrent = 310,   OpHrsPump = 311,    FWVersion = 312,
            DrvVoltage = 313,   OpHrsElec = 314,    NominalSpd = 315,
            DrvPower = 316,     PumpCycles = 319,   TempElec = 326,
            TempPmpBot = 330,   AccelDecel = 336,   TempBearing = 342,
            TempMotor = 346,    ElecName = 349,     HWVersion = 354,
            ErrHist1 = 360,     ErrHist2 = 361,     ErrHist3 = 362,
            ErrHist4 = 363,     ErrHist5 = 364,     ErrHist6 = 365,
            ErrHist7 = 366,     ErrHist8 = 367,     ErrHist9 = 368,
            ErrHist10 = 369,    SetRotSpdRPM = 397, ActualSpdRPM = 398,
            NominalSpdRPM = 399,RUTimeSVal = 700,   SpdSwPt1 = 701,
            SpdSVal = 707,      PwrSVal = 708,      SwoffBKP = 710,
            SwonBKP = 711,      StdbySVal = 717,    SpdSwPt2 = 719,
            VentSpd = 720,      VentTime = 721,     NomSpdConf = 777,   
            RS485Addr = 797
        };
        
        enum class TC110_DOconfigs : short unsigned
        {
            Rot_Speed_Switch_Point_Attained = 0,
            No_Error = 1,
            Error = 2,
            Warning = 3,
            Error_andor_warning = 4,
            Set_speed_attained = 5,
            Pump_on = 6,
            Pump_accelerates = 7,
            Pump_decelerates = 8,
            Always_0 = 9,
            Always_1 = 10,
            Remote_priority_active = 11,
            Heating = 12,
            Backing_pump = 13,
            Sealing_gas = 14,
            Pumping_station = 15
        };
        
        enum class TC110_DIconfigs : short unsigned
        {
            Deactivated = 0,
            Enable_Venting = 1,
            Heating = 2,
            Sealing_gas = 3,
            Run_Up_Time_Control = 4,
            Rot_Speed_Set_Mode = 5,
            Motor = 6
        };
        
        enum class TC110_AOconfigs : short unsigned
        {
            Actual_Rot_Speed =  0,
            Power = 1,
            Current = 2,
            Always_0V = 3,
            Always_10V = 4
        };
        
        enum class TC110_ControlInterfaces 
        {
            Remote = 1,
            RS485 = 2,
            PV_can = 4,
            Fieldbus = 8,
            E74 = 16,
            Unlock = 255
        };
        
        enum class TC110_Accconfigs: short unsigned
        {
            Fan = 0,
            Venting_normallyclosed = 1,
            Heating = 2,
            Backing_pump = 3,
            Fan_tempcontrolled = 4,
            Sealing_gas = 5,
            Always_0 = 6,
            Always_1 = 7,
            Power_failure_venting_unit
        };
        
        enum class TC110_BackingPumpOperatingModes : short unsigned
        {
            Continuous_operation = 0,
            Intermittent_mode = 1,
            Delayed_Switchon = 2
        };
        
        enum class TC110_GasModes : short unsigned
        {
            Heavy_gas = 0,
            Light_gas = 1,
            Helium = 2
        };
        
        enum class TC110_VentModes : short unsigned
        {
            Delayed_Venting = 0,
            No_Venting = 1,
            Direct_Venting = 2
        };
        
        class TC110 : public PfeifferDevice
        {
            RTTR_ENABLE()
        public:
            TC110(shared_ptr<SerialProtocol> proto);
            
            
            bool getHeating();
            void setHeating(bool onoff);
            
            bool getStandby();
            void setStandby(bool onoff);
            
            bool getRUTimeCtrl();
            void setRUTimeCtrl(bool onoff);
            
            bool getPumpStationOnOff();
            void setPumpStationOnOff(bool onoff);
            
            bool getVentEnable();
            void setVentEnable(bool onoff);
            
            short unsigned getSpeedSwitchPointConfig();
            void setSpeedSwitchPointConfig(unsigned short switchpt);
            
            TC110_DOconfigs getDOConfig(unsigned char DOnum);
            void setDOConfig(unsigned char DOnum, TC110_DOconfigs cfg);

            bool getMotorPump();
            void setMotorPump(bool onoff);

            TC110_BackingPumpOperatingModes getBackingPumpMode();
            void setBackingPumpMode(TC110_BackingPumpOperatingModes mode);
            
            bool getRotSpeedSettingMode();
            void setRotSpeedSettingMode(bool mode);
            
            TC110_GasModes getGasMode();
            void setGasMode(TC110_GasModes mode);
            
            TC110_VentModes getVentMode();
            void setVentMode(TC110_VentModes mode);
            
            TC110_Accconfigs getAccConfig(char Acclet, unsigned char Accnum);
            void setAccConfig(char Acclet, unsigned char Accnum, TC110_Accconfigs config);
            
            bool getSealingGas();
            void setSealingGas(bool onoff);
            
            TC110_AOconfigs getAOConfig();
            void setAOConfig(TC110_AOconfigs config);
            
            TC110_ControlInterfaces getCtrlInterface();
            void setCtrlInterface(TC110_ControlInterfaces control);
            
            bool getInterfaceSelectionLocked();
            void setInterfaceSelectionLocked(bool onoff);
            
            TC110_DIconfigs getDIConfig(unsigned char DInum);
            void setDIConfig(unsigned char DInum, TC110_DIconfigs config);
            
            bool getRemotePriority();
            bool getSpeedSwitchPointAttaned();
            
            bool getExcessTemperatureDriveUnit();
            bool getExcessTemperaturePump();
            
            bool getSpeedAttained();
            bool getPumpAccelerates();
            
            
            int getSetRotSpeed();
            int getActualRotSpeed();
            double getDriveCurrent();
            int getOperatingHoursPump();
            double getDriveVoltage();
            int getOperatingHoursDriveUnit();

#pragma once


#include <memory>
#include <string>
#include <optional>
#include <iomanip>
#include <string_view>

#include <foxtrot/Logging.h>
#include <foxtrot/CmdDevice.h>
#include <foxtrot/protocols/SerialProtocol.h>

using std::shared_ptr;
using std::string;
using std::optional;


namespace foxtrot
{
namespace devices
{
    
  using string_view = std::basic_string_view<char>;
    enum class pfeiffer_action : short unsigned
    {
        read = 0,
        describe = 10
    };
    
    
    enum class pfeiffer_data_types : short unsigned
    {
        boolean = 0,
        positive_integer_long = 1,
        positive_fixed_comma_number = 2,
        symbol_chain_short  = 4,
	  new_boolean = 6,
        positive_integer_short = 7,
        symbol_chain_long = 11
    };
    
    class PfeifferDevice : public CmdDevice
    {
    RTTR_ENABLE()
    public:
        virtual string cmd(const string& request) override;
    protected:
        PfeifferDevice(shared_ptr<SerialProtocol> proto, const string& logname);
        string semantic_command(short unsigned address, short unsigned parameter,
                            pfeiffer_action readwrite, optional<string_view> data = std::nullopt);
        
        template<typename T>
        string semantic_command(short unsigned address, T parameter,
                                pfeiffer_action readwrite, optional<string_view> data = std::nullopt)
        {
            return semantic_command(address, static_cast<short unsigned>(parameter), readwrite, data);
        }
        
        shared_ptr<SerialProtocol> _serproto;
        foxtrot::Logging _lg;
        
        std::tuple<int,int,string> interpret_response_telegram(const string& response);
        
        template <typename T> 
        void validate_response_telegram_parameters(unsigned short address, T parameter, 
                                                   const std::tuple<int,int,string>& resp)
        {
            validate_response_telegram_parameters(address, static_cast<short unsigned>(parameter), resp);
        }
        
        void validate_response_telegram_parameters(unsigned short address, unsigned short parameter, const std::tuple<int,int,string>& resp);
        
        template<typename T> 
        string str_from_number(const T& number, unsigned short width)
        {
            std::ostringstream oss;
            oss << std::setw(width) << std::setfill('0') << number;
            return oss.str();
        };
        
        template<typename T>
        string str_from_number(const T& number, pfeiffer_data_types dtype)
        {
            
            using foxtrot::devices::pfeiffer_data_types;
            
            std::ostringstream oss;
            switch(dtype)
            {
                case(pfeiffer_data_types::boolean):
                    oss << std::setw(1);
                    for(int i=0; i< 6; i++)
                        oss << number;
                    break;
	    case(pfeiffer_data_types::new_boolean):
	      oss << std::setw(1);
	      oss << number;
	      
	      break;
                case(pfeiffer_data_types::positive_integer_long):
                case(pfeiffer_data_types::positive_fixed_comma_number):
                case(pfeiffer_data_types::symbol_chain_short):
                    oss << std::setw(6) << std::setfill('0') << number;
                    break;
                case(pfeiffer_data_types::positive_integer_short):
                    oss << std::setw(3) <<  std::setfill('0') << number;
                    break;
                case(pfeiffer_data_types::symbol_chain_long):
                    oss << std::setw(16) << std::setfill(' ') << number;
                default:
                    throw std::logic_error("invalid pfieffer data type!");
                    
            }
            return oss.str();
        }
        
        
        
        
        template<typename T>
        string read_cmd_helper(int addr, T param)
        {
            auto ret = semantic_command(addr, param, pfeiffer_action::read);
            auto interpret = interpret_response_telegram(ret);
            
            validate_response_telegram_parameters(addr, param, interpret);

            return std::get<2>(interpret);
        };
        
        template<typename T1, typename T2>
        void write_cmd_helper(int addr, T1 param, T2 value, pfeiffer_data_types dtype)
        {
            auto st = str_from_number(value, dtype);
            auto ret = semantic_command(addr, param, pfeiffer_action::describe, st);
            
            auto interpret = interpret_response_telegram(ret);
            validate_response_telegram_parameters(addr, param, interpret);
            
        };
        
        
    private:
        string calculate_checksum(string::const_iterator start, string::const_iterator end);
        string calculate_checksum(const string_view message);
        
        std::ostringstream _cmdoss;
        std::ostringstream _semanticoss;
        
    };
    
    
    double pfeiffer_interpret_u_expo_raw(const string& val);
    
}
}
	  int getNominalRotSpeed();

	    double getDrivePower();
	  int getPumpCycles();
	  double getTempElec();
	  double getTempPumpBottom();
	  double getAcceleration();
	  double getTempBearing();
	  double getTempMotor();

	  
	  int getRUTimeSVal();
	  void setRUTimeSVal(int RUTime);

	  int getPowerPercent();
	  void setPowerPercent(int power);

	  int getVentTime();
	  void setVentTime(int vent_time);

	  
	  
	    
            
            
        private:
            short unsigned _address = 1;
            
        };
        
    }
}
