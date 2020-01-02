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
            GasMode = 27,       CfgAccA1 = 35,      CfgAccB1 = 36,
            CfgAccA2 = 37,      CfgAccB2 = 38,      SealingGas = 50,
            CfgAO1 = 55,        CtrlViaInt = 60,    IntSelLckd = 61,
            CfgDI1 = 62,        CfgDI2 = 63,        RemotePrio = 300,
            SpdSwPtAtt = 302,   ErrorCode = 303,    OvTempElec = 304,
            OvTempPump = 305,   SetSpdAtt = 306,    PumpAccel = 307,
            SetRotSpdHz = 308,  ActualSpdHz = 309,  DrvCurrent = 310,
            OpHrsPump = 311,    FWVersion = 312,    DrvVoltage = 313,
            OpHrsElec = 314,    NominalSpd = 315,   DrvPower = 316,
            PumpCycles = 319,   TempElec = 326,     TempPmpBot = 330,
            AccelDecel = 336,   TempBearing = 342,  TempMotor = 346,
            ElecName = 349,     HWVersion = 354,    ErrHist1 = 360,
            ErrHist2 = 361,     ErrHist3 = 362,     ErrHist4 = 363,
            ErrHist5 = 364,     ErrHist6 = 365,     ErrHist7 = 366,
            ErrHist8 = 367,     ErrHist9 = 368,     ErrHist10 = 369,
            SetRotSpdRPM = 397, ActualSpdRPM = 398, NominalSpdRPM = 399,
            RUTimeSVal = 700,   SpdSwPt1 = 701,     SpdSVal = 707,
            PwdSVal = 708,      SwoffBKP = 710,     SwonBKP = 711,
            StdbySVal = 717,    SpdSwPt2 = 719,     VentSpd = 720,
            VentTime = 721,     NomSpdConf = 777,   RS485Addr = 797
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
            void setPumpstationOnOff(bool onoff);
            
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
        
            
        private:
            short unsigned _address = 1;
            
        };
        
    }
}
