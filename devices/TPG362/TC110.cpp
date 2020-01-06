#include "TC110.h"

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

void foxtrot::devices::TC110::setPumpstationOnOff(bool onoff)
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


