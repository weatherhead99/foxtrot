#include "TC110.h"
#include <rttr/registration>

using namespace foxtrot::devices;

foxtrot::devices::TC110::TC110(shared_ptr<foxtrot::SerialProtocol> proto)
: PfeifferDevice(proto, "TC110")
{
}

bool foxtrot::devices::TC110::getHeating()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::Heating));
}

void foxtrot::devices::TC110::setHeating(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::Heating, onoff);
}


bool foxtrot::devices::TC110::getStandby()
{
  return std::stoul(read_cmd_helper(_address, TC110_parameter_no::Standby));   
}

void foxtrot::devices::TC110::setStandby(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::Standby, onoff);
}

bool foxtrot::devices::TC110::getRUTimeCtrl()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::RUTimeCtrl));
}

void foxtrot::devices::TC110::setRUTimeCtrl(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::RUTimeCtrl, onoff);
}

bool foxtrot::devices::TC110::getPumpStationOnOff()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::PumpgStatn));
}

void foxtrot::devices::TC110::setPumpStationOnOff(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::PumpgStatn, onoff);
}

bool foxtrot::devices::TC110::getVentEnable()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::EnableVent));
}

void foxtrot::devices::TC110::setVentEnable(bool onoff)
{   
    write_cmd_helper(_address, TC110_parameter_no::EnableVent, onoff);
}

short unsigned foxtrot::devices::TC110::getSpeedSwitchPointConfig()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::CfgSpdSwPt));
}

void foxtrot::devices::TC110::setSpeedSwitchPointConfig(unsigned short switchpt)
{
    write_cmd_helper(_address, TC110_parameter_no::CfgSpdSwPt, switchpt);
}

foxtrot::devices::TC110_parameter_no get_DOparam(unsigned char DOnum)
{
    foxtrot::devices::TC110_parameter_no param;
    switch(DOnum)
    {
        case(1):  param = foxtrot::devices::TC110_parameter_no::CfgDO1;  break;
        case(2):  param = foxtrot::devices::TC110_parameter_no::CfgDO2; break;
        default:
            throw std::out_of_range("invalid DO number");
    }
    return param;
}


foxtrot::devices::TC110_DOconfigs foxtrot::devices::TC110::getDOConfig(unsigned char DOnum)
{
    auto ret = std::stoul(read_cmd_helper(_address, get_DOparam(DOnum)));
    TC110_DOconfigs out{static_cast<short unsigned>(ret)};
    return out;
}

void foxtrot::devices::TC110::setDOConfig(unsigned char DOnum, foxtrot::devices::TC110_DOconfigs cfg)
{
    write_cmd_helper(_address, get_DOparam(DOnum),cfg);
}

bool foxtrot::devices::TC110::getMotorPump()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::MotorPump));
}

void foxtrot::devices::TC110::setMotorPump(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::MotorPump, onoff);
}

foxtrot::devices::TC110_BackingPumpOperatingModes foxtrot::devices::TC110::getBackingPumpMode()
{
    auto ret = std::stoul(read_cmd_helper(_address, TC110_parameter_no::OpModeBKP));
    TC110_BackingPumpOperatingModes out{static_cast<short unsigned>(ret)};
    return out;
}

void foxtrot::devices::TC110::setBackingPumpMode(foxtrot::devices::TC110_BackingPumpOperatingModes mode)
{
    write_cmd_helper(_address, TC110_parameter_no::OpModeBKP, mode);
}

bool foxtrot::devices::TC110::getRotSpeedSettingMode()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::SpdSetMode));
}

void foxtrot::devices::TC110::setRotSpeedSettingMode(bool mode)
{
    write_cmd_helper(_address, TC110_parameter_no::SpdSetMode, mode);
}

foxtrot::devices::TC110_GasModes foxtrot::devices::TC110::getGasMode()
{
    auto ret = std::stoul(read_cmd_helper(_address, TC110_parameter_no::GasMode));
    TC110_GasModes out{static_cast<short unsigned>(ret)};
    return out;
}

void foxtrot::devices::TC110::setGasMode(foxtrot::devices::TC110_GasModes mode)
{
    write_cmd_helper(_address, TC110_parameter_no::GasMode, mode);
}

foxtrot::devices::TC110_VentModes foxtrot::devices::TC110::getVentMode()
{
    auto ret = std::stoul(read_cmd_helper(_address, TC110_parameter_no::VentMode));
    TC110_VentModes out{static_cast<short unsigned>(ret)};
    return out;
}

void foxtrot::devices::TC110::setVentMode(foxtrot::devices::TC110_VentModes mode)
{
    write_cmd_helper(_address, TC110_parameter_no::VentMode, mode);
}

foxtrot::devices::TC110_parameter_no get_Accparam(char Acclet, unsigned char Accnum)
{
    foxtrot::devices::TC110_parameter_no param;
    switch(Acclet)
    {
        case('A'): 
        case('a'):
            switch(Accnum)
            {
                case(1): param = TC110_parameter_no::CfgAccA1; break;
                case(2): param = TC110_parameter_no::CfgAccA2; break;
                default: throw std::out_of_range("invalid accessory number parameter");
            }
        break;
        
        case('B') :
        case('b') :
            switch(Accnum)
            {
                case(1): param = TC110_parameter_no::CfgAccB1; break;
                case(2): param = TC110_parameter_no::CfgAccB2; break;
                default: throw std::out_of_range("invalid accessory number parameter");
            }
        break; 
        
        default:
            throw std::out_of_range("invalid Accessory letter parameter");
                
    }
    
    return param;
}

foxtrot::devices::TC110_Accconfigs foxtrot::devices::TC110::getAccConfig(char Acclet, unsigned char Accnum)
{
    TC110_parameter_no param = get_Accparam(Acclet, Accnum);
    auto ret = std::stoul(read_cmd_helper(_address, param));
    TC110_Accconfigs out{static_cast<short unsigned>(ret)};
    return out;
}

void foxtrot::devices::TC110::setAccConfig(char Acclet, unsigned char Accnum, foxtrot::devices::TC110_Accconfigs config)
{
    TC110_parameter_no param = get_Accparam(Acclet, Accnum);
    write_cmd_helper(_address, param, config);
}

bool foxtrot::devices::TC110::getSealingGas()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::SealingGas));
}

void foxtrot::devices::TC110::setSealingGas(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::SealingGas, onoff);
}


int foxtrot::devices::TC110::getSetRotSpeed()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::SetRotSpdHz));
};

int foxtrot::devices::TC110::getActualRotSpeed()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::ActualSpdHz));
}

bool foxtrot::devices::TC110::getSpeedAttained()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::SetSpdAtt));
}

double foxtrot::devices::TC110::getDriveVoltage()
{
    return std::stod(read_cmd_helper(_address, TC110_parameter_no::DrvVoltage));
}

double foxtrot::devices::TC110::getDriveCurrent()
{
    return std::stod(read_cmd_helper(_address, TC110_parameter_no::DrvCurrent));
}

bool foxtrot::devices::TC110::getExcessTemperatureDriveUnit()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::OvTempElec));
}

bool foxtrot::devices::TC110::getExcessTemperaturePump()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::OvTempPump));
}

bool foxtrot::devices::TC110::getPumpAccelerates()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::PumpAccel));
}

int foxtrot::devices::TC110::getOperatingHoursDriveUnit()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::OpHrsElec));
}

int foxtrot::devices::TC110::getOperatingHoursPump()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::OpHrsPump));
}






RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::TC110;

  using foxtrot::devices::TC110_DOconfigs;
  registration::enumeration<TC110_DOconfigs>
    ("foxtrot::devices::TC110_DOconfigs")
    (value("Rot_Speed_Switch_Point_Attained",
	   TC110_DOconfigs::Rot_Speed_Switch_Point_Attained),
     value("No_Error", TC110_DOconfigs::No_Error)
     );
  
//   registration::class_<TC110>("foxtrot::devices::TC110")
//     .property("Heating", &TC110::getHeating,
// 	      &TC110::setHeating)
//     .property("Standby", &TC110::getStandby,
// 	      &TC110::setStandby)
//     .property("RUTimeCtrl", &TC110::getRUTimeCtrl,
// 	      &TC110::setRUTimeCtrl)
//     .property("PumpStationOnOff", &TC110::getPumpStationOnOff,
// 	      &TC110::setPumpStationOnOff)
//     .property("VentEnable", &TC110::getVentEnable,
// 	      &TC110::setVentEnable)

   
    
//     .property("SpeedSwitchPointConfig", &TC110::getSpeedSwitchPointConfig,
// 	      &TC110::setSpeedSwitchPointConfig)

    //TODO::DOConfigs

//     .property("MotorPump", &TC110::getMotorPump,
// 	      &TC110::setMotorPump)

    //TODO: TC110_backingpumpoperatingmodes

//     .property("BackingPumpMode", &TC110::getBackingPumpMode,
// 	      &TC110::setBackingPumpMode)
// 
//     .property("RotSpeedSettingMode", &TC110::getRotSpeedSettingMode,
// 	      &TC110::setRotSpeedSettingMode)
// 
//     .property("GasMode", &TC110::getGasMode,
// 	      &TC110::setGasMode)
// 
//     .property("VentMode", &TC110::getVentMode,
// 	      &TC110::setVentMode)

    //TODO: AccConfig

//     .property("SealingGas", &TC110::getSealingGas,
// 	      &TC110::setSealingGas)

    //.property("AOConfig", &TC110::getAOConfig,
	//      &TC110::setAOConfig)

    //.property("CtrlInterface", &TC110::getCtrlInterface,
	//      &TC110::setCtrlInterface)


    //.property("InterfaceSelectionLocked",
// 	      &TC110::getInterfaceSelectionLocked,
// 	      &TC110::setInterfaceSelectionLocked)

    
    //TODO: DIConfig

    //.property_readonly("RemotePriority", &TC110::getRemotePriority)

    .property_readonly("ExcessTemperatureDriveUnit", &TC110::getExcessTemperatureDriveUnit)
    .property_readonly("ExcessTemperaturePump", &TC110::getExcessTemperaturePump)
    .property_readonly("SpeedAttained", &TC110::getSpeedAttained)
    .property_readonly("PumpAccelerates", &TC110::getPumpAccelerates)
    .property_readonly("SetRotSpeed", &TC110::getSetRotSpeed)
    .property_readonly("ActualRotSpeed", &TC110::getActualRotSpeed)
    .property_readonly("DriveCurrent", &TC110::getDriveCurrent)
    .property_readonly("OperatingHoursPump", &TC110::getOperatingHoursPump)
    .property_readonly("DriveVoltage", &TC110::getDriveVoltage)
    .property_readonly("OperatingHoursDriveUnit", &TC110::getOperatingHoursDriveUnit)
    ;

}
