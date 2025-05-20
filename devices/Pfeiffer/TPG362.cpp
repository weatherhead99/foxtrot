
#include <iostream>
#include <numeric>
#include <iomanip>
#include <chrono>
#include <thread>
#include <climits>

#include <rttr/registration>

#include <foxtrot/DeviceError.h>

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/protocols/simpleTCP.h>

#include "TPG362.h"


foxtrot::devices::TPG362::TPG362(std::shared_ptr< foxtrot::SerialProtocol > proto)
  : PfeifferDevice(proto, "TPG362")
{


  if(typeid(_serproto) == typeid(foxtrot::protocols::SerialPort))
  {
    _lg.Info("using serial connected gauge controller");
    _serproto->Init(nullptr);
    auto specproto = std::static_pointer_cast<foxtrot::protocols::SerialPort>(_serproto);
    specproto->flush();
    _lg.Info("gauge controller initialization done...");
  }
  else if(typeid(_serproto) == typeid(foxtrot::protocols::simpleTCP))
    {
    throw foxtrot::DeviceError("ethernet not supported... yet");
    }
  else
  {
    throw std::logic_error("got unexpected serial interface type!");
  };
  
}

double foxtrot::devices::TPG362::getPressure(short unsigned int channel)
{
  auto interpret = read_cmd_helper(channel + _address, TPG_parameter_no::Pressure);
  return pfeiffer_interpret_u_expo_raw(interpret);
}

string foxtrot::devices::TPG362::getDeviceName(short unsigned int channel)
{
  return read_cmd_helper(channel + _address, TPG_parameter_no::DeviceName);
}

bool foxtrot::devices::TPG362::getGaugeOnOff(short unsigned int channel)
{
  auto interpret = read_cmd_helper(channel + _address, TPG_parameter_no::sensEnable);   
  return static_cast<bool>(std::stoi(interpret));
}

void foxtrot::devices::TPG362::setGaugeOnOff(unsigned short channel, bool onoff)
{
    write_cmd_helper(channel + _address, TPG_parameter_no::sensEnable, onoff, pfeiffer_data_types::positive_integer_short);
}

bool foxtrot::devices::TPG362::getDegass(unsigned short channel)
{
    auto interpret = read_cmd_helper(channel + _address, TPG_parameter_no::degas);
    return static_cast<bool>(std::stoi(interpret));
}

void foxtrot::devices::TPG362::setDegass(unsigned short channel, bool onoff)
    {
    write_cmd_helper(channel + _address, TPG_parameter_no::degas, onoff, pfeiffer_data_types::new_boolean);
}



const string foxtrot::devices::TPG362::getDeviceTypeName() const
{
  return "TPG362";
}



short unsigned convert_int_to_short_unsigned(int value, bool& ok)
{
    if(value < 0)
    {
        ok = false;
        return 0;
    }
    else if(value > USHRT_MAX)
    {
        ok = false;
        return 0;
    }
//     std::cout << "in value: " << value << std::endl;
    
    return static_cast<short unsigned>(value);   
}

RTTR_REGISTRATION{
    using namespace rttr;
    using foxtrot::devices::TPG362;
    registration::class_<TPG362>("foxtrot::devices::TPG362")
    .method("getPressure", &TPG362::getPressure)
    (
        parameter_names("channel")
    )
    .method("getDeviceName", &TPG362::getDeviceName)
    (
        parameter_names("channel")
    )
    .method("getGaugeOnOff",&TPG362::getGaugeOnOff)
    (
        parameter_names("channel")
    )
    .method("setGaugeOnOff",&TPG362::setGaugeOnOff)
    (
        parameter_names("channel","onoff")
    )
    .method("getDegass", &TPG362::getDegass)
    (
        parameter_names("channel")
        )
    .method("setDegass",&TPG362::setDegass)
    (        parameter_names("channel","onoff")
        );
    
    rttr::type::register_converter_func(convert_int_to_short_unsigned);
}
