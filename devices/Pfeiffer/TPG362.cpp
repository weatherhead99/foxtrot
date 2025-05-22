
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

  const auto& tpinfo = typeid(*(proto.get()));
  _lg.strm(sl::trace) << "name of protocol type: " << tpinfo.name() ;
  _serproto->Init(nullptr);
  
  if(tpinfo == typeid(foxtrot::protocols::SerialPort))
  {
    _lg.Info("using serial connected gauge controller");
    auto specproto = std::static_pointer_cast<foxtrot::protocols::SerialPort>(_serproto);
    specproto->flush();
    _lg.Info("gauge controller initialization done...");
  }
  else if(tpinfo == typeid(foxtrot::protocols::simpleTCP))
    {
      _lg.strm(sl::info) << "using ethernet/LAN connected gauge controller";
      _lg.strm(sl::info) << "gauge controller initialization done...";
    }
  else
  {
    throw std::logic_error("got unexpected serial interface type!");
  };

  //determine number of gauge channels (by looking at device name)
  auto devname = getDeviceName(0);

  _lg.strm(sl::info) << "device type is: " << devname;
  _lg.strm(sl::trace) << "length of devname: " << devname.size();
  char chancar = devname[devname.size()-1];
  _lg.strm(sl::trace) << "chancar: " << (int) chancar;
    _devchannels = (int) (chancar - '0');
  _lg.strm(sl::debug) << "number of channels: " << _devchannels;
  

  
}

double foxtrot::devices::TPG362::getPressure(short unsigned int channel)
{
  checkChannel(channel);
  auto interpret = read_cmd_helper(channel + _address, TPG_parameter_no::Pressure);
  return pfeiffer_interpret_u_expo_raw(interpret);
}

string foxtrot::devices::TPG362::getDeviceName(short unsigned int channel)
{
  checkChannel(channel);
  return read_cmd_helper(channel + _address, TPG_parameter_no::DeviceName);
}

bool foxtrot::devices::TPG362::getGaugeOnOff(short unsigned int channel)
{
  checkChannel(channel);
  auto interpret = read_cmd_helper(channel + _address, TPG_parameter_no::sensEnable);   
  return static_cast<bool>(std::stoi(interpret));
}

void foxtrot::devices::TPG362::setGaugeOnOff(unsigned short channel, bool onoff)
{
    checkChannel(channel);
    write_cmd_helper(channel + _address, TPG_parameter_no::sensEnable, onoff, pfeiffer_data_types::positive_integer_short);
}

bool foxtrot::devices::TPG362::getDegass(unsigned short channel)
{
    checkChannel(channel);
    auto interpret = read_cmd_helper(channel + _address, TPG_parameter_no::degas);
    return static_cast<bool>(std::stoi(interpret));
}

void foxtrot::devices::TPG362::setDegass(unsigned short channel, bool onoff)
    {
        checkChannel(channel);
    write_cmd_helper(channel + _address, TPG_parameter_no::degas, onoff, pfeiffer_data_types::new_boolean);
}


std::string foxtrot::devices::TPG362::SerialNumber()
{
  return read_cmd_helper(_address, TPG_parameter_no::SerialNumber);
}

std::string foxtrot::devices::TPG362::OrderNumber()
{
  return read_cmd_helper(_address, TPG_parameter_no::OrderNumber);
}

std::string foxtrot::devices::TPG362::HWVersion()
{
 return read_cmd_helper(_address, TPG_parameter_no::HWVersion);
}

std::string foxtrot::devices::TPG362::FWVersion()
{
  return read_cmd_helper(_address, TPG_parameter_no::FWVersion);
}

unsigned foxtrot::devices::TPG362::OperatingHours()
{
  auto itp = read_cmd_helper(_address, TPG_parameter_no::OperatHours);
  return static_cast<unsigned>(std::stoi(itp));
}

std::string foxtrot::devices::TPG362::Error(short unsigned channel)
{
  checkChannel(channel);
  return read_cmd_helper(_address + channel, TPG_parameter_no::ErrorCode);

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
