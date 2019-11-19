
#include <iostream>
#include <numeric>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <thread>
#include <climits>

#include <rttr/registration>

#include <foxtrot/DeviceError.h>

#include <foxtrot/protocols/SerialPort.h>

#include "TPG362.h"


foxtrot::devices::TPG362::TPG362(std::shared_ptr< foxtrot::SerialProtocol > proto)
  : PfeifferDevice(proto, "TPG362")
{
  
  auto specproto = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(_serproto);
  if(specproto != nullptr)
  {
    _lg.Info("using serial connected gauge controller");
    _serproto->Init(nullptr);
    specproto->flush();
    _lg.Info("gauge controller initialization done...");
  }
  else
  {
    throw foxtrot::DeviceError("ethernet not supported... yet");
  };
  
}

double foxtrot::devices::TPG362::getPressure(short unsigned int channel)
{
  auto interpret = read_cmd_helper(channel, TPG_parameter_no::Pressure);
  return interpret_u_expo_raw(std::get<2>(interpret));
}

string foxtrot::devices::TPG362::getDeviceName(short unsigned int channel)
{
  auto interpret = read_cmd_helper(channel, TPG_parameter_no::DeviceName);
  return std::get<2>(interpret);
}

bool foxtrot::devices::TPG362::getGaugeOnOff(short unsigned int channel)
{
  auto interpret = read_cmd_helper(channel, TPG_parameter_no::sensEnable);   
  return static_cast<bool>(std::stoi(std::get<2>(interpret)));
}

void foxtrot::devices::TPG362::setGaugeOnOff(unsigned short channel, bool onoff)
{
    write_cmd_helper(channel, TPG_parameter_no::sensEnable, onoff);
}

bool foxtrot::devices::TPG362::getDegass(unsigned short channel)
{
    auto interpret = read_cmd_helper(channel, TPG_parameter_no::degas);
    return static_cast<bool>(std::stoi(std::get<2>(interpret)));
}

void foxtrot::devices::TPG362::setDegass(unsigned short channel, bool onoff)
{
    write_cmd_helper(channel, TPG_parameter_no::degas, onoff);
}



const string foxtrot::devices::TPG362::getDeviceTypeName() const
{
  return "TPG362";
}



double foxtrot::devices::TPG362::interpret_u_expo_raw(const string& val)
{
  double mantissa = std::stoi(val.substr(0,4))/ 1000.;
  short exponent = std::stoi(val.substr(4,2)) - 20;
  return mantissa * std::pow(10,exponent);
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
    std::cout << "in value: " << value << std::endl;
    
    return static_cast<short unsigned>(value);   
}

RTTR_REGISTRATION{
    using namespace rttr;
    using foxtrot::devices::TPG362;
    registration::class_<TPG362>("foxtrot::devices::dummyDevice")
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
    (
        parameter_names("channel","onoff")
        );
    
    rttr::type::register_converter_func(convert_int_to_short_unsigned);
}
