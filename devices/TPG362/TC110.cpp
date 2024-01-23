#include "TC110.h"
#include <rttr/registration>
#include <foxtrot/protocols/SerialPort.h>
#include <map>



using namespace foxtrot::devices;


const std::map<TC110_parameter_no, unsigned short> data_types_map = {};


foxtrot::devices::TC110::TC110(shared_ptr<foxtrot::SerialProtocol> proto)
: PfeifferDevice(proto, "TC110")
{
  auto specproto = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(proto);
  if(specproto == nullptr)
    throw std::logic_error("invalid type of protocol passed to TC110 initializer");

  
  _lg.Info("using RS485 connected vacuum pump controller");
  specproto->Init(nullptr);
  specproto->flush();
  _lg.Info("serial initialization is now done");
  
}

bool foxtrot::devices::TC110::getHeating()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::Heating));
}

void foxtrot::devices::TC110::setHeating(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::Heating, onoff, pfeiffer_data_types::boolean);
}


bool foxtrot::devices::TC110::getStandby()
{
  return std::stoul(read_cmd_helper(_address, TC110_parameter_no::Standby));   
}

void foxtrot::devices::TC110::setStandby(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::Standby, onoff, pfeiffer_data_types::boolean);
}

bool foxtrot::devices::TC110::getRUTimeCtrl()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::RUTimeCtrl));
}

void foxtrot::devices::TC110::setRUTimeCtrl(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::RUTimeCtrl, onoff, pfeiffer_data_types::boolean);
}

bool foxtrot::devices::TC110::getPumpStationOnOff()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::PumpgStatn));
}

void foxtrot::devices::TC110::setPumpStationOnOff(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::PumpgStatn, onoff, pfeiffer_data_types::boolean);
}

bool foxtrot::devices::TC110::getVentEnable()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::EnableVent));
}

void foxtrot::devices::TC110::setVentEnable(bool onoff)
{   
    write_cmd_helper(_address, TC110_parameter_no::EnableVent, onoff, pfeiffer_data_types::boolean);
}

short unsigned foxtrot::devices::TC110::getSpeedSwitchPointConfig()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::CfgSpdSwPt));
}

void foxtrot::devices::TC110::setSpeedSwitchPointConfig(unsigned short switchpt)
{
    write_cmd_helper(_address, TC110_parameter_no::CfgSpdSwPt, switchpt, pfeiffer_data_types::positive_integer_short);
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
    write_cmd_helper(_address, get_DOparam(DOnum),static_cast<unsigned short>(cfg), pfeiffer_data_types::positive_integer_short);
}

bool foxtrot::devices::TC110::getMotorPump()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::MotorPump));
}

void foxtrot::devices::TC110::setMotorPump(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::MotorPump, onoff, pfeiffer_data_types::boolean);
}

foxtrot::devices::TC110_BackingPumpOperatingModes foxtrot::devices::TC110::getBackingPumpMode()
{
    auto ret = std::stoul(read_cmd_helper(_address, TC110_parameter_no::OpModeBKP));
    TC110_BackingPumpOperatingModes out{static_cast<short unsigned>(ret)};
    return out;
}

void foxtrot::devices::TC110::setBackingPumpMode(foxtrot::devices::TC110_BackingPumpOperatingModes mode)
{
    write_cmd_helper(_address, TC110_parameter_no::OpModeBKP, static_cast<short unsigned>(mode), pfeiffer_data_types::positive_integer_short);
}

bool foxtrot::devices::TC110::getRotSpeedSettingMode()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::SpdSetMode));
}

void foxtrot::devices::TC110::setRotSpeedSettingMode(bool mode)
{
    write_cmd_helper(_address, static_cast<short unsigned>(TC110_parameter_no::SpdSetMode), mode, pfeiffer_data_types::positive_integer_short);
}

foxtrot::devices::TC110_GasModes foxtrot::devices::TC110::getGasMode()
{
    auto ret = std::stoul(read_cmd_helper(_address, TC110_parameter_no::GasMode));
    TC110_GasModes out{static_cast<short unsigned>(ret)};
    return out;
}

void foxtrot::devices::TC110::setGasMode(foxtrot::devices::TC110_GasModes mode)
{
    write_cmd_helper(_address, TC110_parameter_no::GasMode, static_cast<unsigned short>(mode), pfeiffer_data_types::positive_integer_short);
}

foxtrot::devices::TC110_VentModes foxtrot::devices::TC110::getVentMode()
{
    auto ret = std::stoul(read_cmd_helper(_address, TC110_parameter_no::VentMode));
    TC110_VentModes out{static_cast<short unsigned>(ret)};
    return out;
}

void foxtrot::devices::TC110::setVentMode(foxtrot::devices::TC110_VentModes mode)
{
    write_cmd_helper(_address, TC110_parameter_no::VentMode, static_cast<unsigned short>(mode), pfeiffer_data_types::positive_integer_short);
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
    write_cmd_helper(_address, param, static_cast<unsigned short>(config), pfeiffer_data_types::positive_integer_short);
}

bool foxtrot::devices::TC110::getSealingGas()
{
    return std::stoul(read_cmd_helper(_address, TC110_parameter_no::SealingGas));
}

void foxtrot::devices::TC110::setSealingGas(bool onoff)
{
    write_cmd_helper(_address, TC110_parameter_no::SealingGas, onoff, pfeiffer_data_types::boolean);
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

int foxtrot::devices::TC110::getNominalRotSpeed()
{

  return std::stoul(read_cmd_helper(_address, TC110_parameter_no::NominalSpd));
}

double foxtrot::devices::TC110::getDrivePower()
{

  return std::stod(read_cmd_helper(_address, TC110_parameter_no::DrvPower));

}

int foxtrot::devices::TC110::getPumpCycles()
{
  return std::stoul(read_cmd_helper(_address, TC110_parameter_no::PumpCycles));
}

double foxtrot::devices::TC110::getTempElec()
{
  return std::stod(read_cmd_helper(_address, TC110_parameter_no::TempElec));
}

double foxtrot::devices::TC110::getTempPumpBottom()
{
  return std::stod(read_cmd_helper(_address, TC110_parameter_no::TempPmpBot));
}

double foxtrot::devices::TC110::getAcceleration()
{
  return std::stod(read_cmd_helper(_address, TC110_parameter_no::AccelDecel));
}

double foxtrot::devices::TC110::getTempBearing()
{
  return std::stod(read_cmd_helper(_address, TC110_parameter_no::TempBearing));
}

double foxtrot::devices::TC110::getTempMotor()
{
  return std::stod(read_cmd_helper(_address, TC110_parameter_no::TempMotor));
}

int foxtrot::devices::TC110::getRUTimeSVal()
{
  return std::stoul(read_cmd_helper(_address, TC110_parameter_no::RUTimeSVal));
}

void foxtrot::devices::TC110::setRUTimeSVal(int RUTime)
{
  if(RUTime < 1 or RUTime > 120)
    throw std::out_of_range("supplied an invalid RUTimeSVal");

  write_cmd_helper(_address, TC110_parameter_no::RUTimeSVal, static_cast<unsigned short>(RUTime),
		   pfeiffer_data_types::positive_integer_long);
}

int foxtrot::devices::TC110::getPowerPercent()
{
  return std::stoul(read_cmd_helper(_address, TC110_parameter_no::PwrSVal));
}

void foxtrot::devices::TC110::setPowerPercent(int power)
{
  if(power < 10 or power > 100)
    throw std::out_of_range("supplied invalid power percent");

  write_cmd_helper(_address, TC110_parameter_no::PwrSVal, static_cast<unsigned short>(power),
		   pfeiffer_data_types::positive_integer_short);
}

int foxtrot::devices::TC110::getVentTime()
{
  return std::stoul(read_cmd_helper(_address, TC110_parameter_no::VentTime));
}

void foxtrot::devices::TC110::setVentTime(int vent_time)
{
  if( vent_time < 0 or vent_time > 3600)
    throw std::out_of_range("supplied invalid vent time");

  write_cmd_helper(_address, TC110_parameter_no::VentTime, static_cast<unsigned short>(vent_time),
		   pfeiffer_data_types::positive_integer_long);
    
}









RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::TC110;

  using foxtrot::devices::TC110_DOconfigs;
  using foxtrot::devices::TC110_parameter_no;
  
  registration::enumeration<TC110_parameter_no>
  ("foxtrot::devices::TC110_parameter_no")
  (value("Heating" , TC110_parameter_no::Heating),
   value("Standby" , TC110_parameter_no::Standby),
   value("RUTimeCtrl", TC110_parameter_no::RUTimeCtrl),
   value("ErrorAckn", TC110_parameter_no::ErrorAckn),
   value("PumpgStatn", TC110_parameter_no::PumpgStatn),
   value("EnableVent", TC110_parameter_no::EnableVent),
   value("CfgSpdSwPt", TC110_parameter_no::CfgSpdSwPt),
   value("CfgDO2", TC110_parameter_no::CfgDO2),
   value("MotorPump", TC110_parameter_no::MotorPump),
   value("CfgDO1", TC110_parameter_no::CfgDO1),
   value("OpModeBKP", TC110_parameter_no::OpModeBKP),
   value("SpdSetMode", TC110_parameter_no::SpdSetMode),
   value("GasMode", TC110_parameter_no::GasMode),
   value("VentMode", TC110_parameter_no::VentMode),
   value("CfgAccA1", TC110_parameter_no::CfgAccA1),
   value("CfgAccB1", TC110_parameter_no::CfgAccB1),
   value("CfgAccA2", TC110_parameter_no::CfgAccA2),
   value("CfgAccB2", TC110_parameter_no::CfgAccB2),
   value("SealingGas", TC110_parameter_no::SealingGas),
   value("CfgAO1", TC110_parameter_no::CfgAO1),
   value("CtrlViaInt", TC110_parameter_no::CtrlViaInt),
   value("IntSelLckd", TC110_parameter_no::IntSelLckd),
   value("CfgDI1", TC110_parameter_no::CfgDI1),
   value("CfgDI2", TC110_parameter_no::CfgDI2),
   value("RemotePrio", TC110_parameter_no::RemotePrio),
   value("SpdSwPtAtt", TC110_parameter_no::SpdSwPtAtt),
   value("ErrorCode", TC110_parameter_no::ErrorCode),
   value("OvTempElec", TC110_parameter_no::OvTempElec),
   value("OvTempPump", TC110_parameter_no::OvTempPump)
   
   );
  
  
  
  
  registration::enumeration<TC110_DOconfigs>
    ("foxtrot::devices::TC110_DOconfigs")
    (value("Rot_Speed_Switch_Point_Attained",
	   TC110_DOconfigs::Rot_Speed_Switch_Point_Attained),
     value("No_Error", TC110_DOconfigs::No_Error)
     );

  registration::enumeration<TC110_BackingPumpOperatingModes>
    ("foxtrot::devices::TC110_BackingPumpOperatingModes")
    (value("Continuous_Operation", TC110_BackingPumpOperatingModes::Continuous_operation),
     value("Intermittent_mode", TC110_BackingPumpOperatingModes::Intermittent_mode),
     value("Delayed_Switchon", TC110_BackingPumpOperatingModes::Delayed_Switchon));

  registration::enumeration<TC110_VentModes>
    ("foxtrot::devices::TC110_VentModes")
    (value("Delayed_Venting", TC110_VentModes::Delayed_Venting),
     value("No_Venting", TC110_VentModes::No_Venting),
     value("Direct_Venting", TC110_VentModes::Direct_Venting));

  
  

  
   registration::class_<TC110>("foxtrot::devices::TC110")
     .property("Heating", &TC110::getHeating,
 	      &TC110::setHeating)
     .property("Standby", &TC110::getStandby,
 	      &TC110::setStandby)
     .property("RUTimeCtrl", &TC110::getRUTimeCtrl,
 	      &TC110::setRUTimeCtrl)
     .property("PumpStationOnOff", &TC110::getPumpStationOnOff,
 	      &TC110::setPumpStationOnOff)
     .property("VentEnable", &TC110::getVentEnable,
 	      &TC110::setVentEnable)
     .property("SpeedSwitchPointConfig", &TC110::getSpeedSwitchPointConfig,
 	      &TC110::setSpeedSwitchPointConfig)

//      .property("DOConfig", &TC110::getDOConfig,
//                &TC110::setDOConfig)
     

     .property("MotorPump", &TC110::getMotorPump,
 	      &TC110::setMotorPump)

     .property("BackingPumpMode", &TC110::getBackingPumpMode,
	       &TC110::setBackingPumpMode)

// 
//     .property("RotSpeedSettingMode", &TC110::getRotSpeedSettingMode,
// 	      &TC110::setRotSpeedSettingMode)
// 
//     .property("GasMode", &TC110::getGasMode,
// 	      &TC110::setGasMode)
// 
      .property("VentMode", &TC110::getVentMode,
		&TC110::setVentMode)

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
      .property_readonly("NominalRotSpeed", &TC110::getNominalRotSpeed)
      .property_readonly("DrivePower", &TC110::getDrivePower)
      .property_readonly("PumpCycles", &TC110::getPumpCycles)
      .property_readonly("TempElec", &TC110::getTempElec)
      .property_readonly("TempPumpBottom", &TC110::getTempPumpBottom)
      .property_readonly("Acceleration", &TC110::getAcceleration)
      .property_readonly("TempBearing", &TC110::getTempBearing)
      .property_readonly("TempMotor", &TC110::getTempMotor)
      .property("RUTimeSVal", &TC110::getRUTimeSVal, &TC110::setRUTimeSVal)
      .property("PowerPercent", &TC110::getPowerPercent, &TC110::setPowerPercent)
      .property("VentTime", &TC110::getVentTime, &TC110::setVentTime)
      
    ;

}
