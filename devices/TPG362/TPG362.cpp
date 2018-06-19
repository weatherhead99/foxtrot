#include "TPG362.h"

#include "DeviceError.h"
#include <SerialPort.h>
#include <iostream>

#include <numeric>
#include <iomanip>

#include <cmath>
#include <chrono>
#include <thread>

#include <rttr/registration>
#include <climits>

foxtrot::devices::TPG362::TPG362(std::shared_ptr< foxtrot::SerialProtocol > proto)
  : CmdDevice(proto), _serproto(proto), _lg("TPG362")
{
  
  auto specproto = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(_serproto);
  if(specproto != nullptr)
  {
    _lg.Info("using serial connected gauge controller");
    _serialmode = true;
    _serproto->Init(nullptr);
    
  }
  else
  {
    throw foxtrot::DeviceError("ethernet not supported... yet");
    _serialmode = false;
  };
  
  
}


double foxtrot::devices::TPG362::getPressure(short unsigned int channel)
{
  
  auto ret = semantic_cmd(channel,parameter_no::Pressure,action::read);
  
  
  auto interpret = interpret_response_telegram(ret);
  validate_response_telegram_parameters(channel,parameter_no::Pressure,interpret);
  
  return interpret_u_expo_raw(std::get<2>(interpret));

}


string foxtrot::devices::TPG362::getDeviceName(short unsigned int channel)
{
  auto ret = semantic_cmd(channel,parameter_no::DeviceName, action::read);
  auto interpret = interpret_response_telegram(ret);
  validate_response_telegram_parameters(channel,parameter_no::DeviceName,interpret);
  
  return std::get<2>(interpret);

}

bool foxtrot::devices::TPG362::getGaugeOnOff(short unsigned int channel)
{
  auto ret = semantic_cmd(channel,parameter_no::sensEnable,action::read);
  auto interpret = interpret_response_telegram(ret);
  validate_response_telegram_parameters(channel,parameter_no::sensEnable,interpret);
  
  
  return static_cast<bool>(std::stoi(std::get<2>(interpret)));
  
  
}

void foxtrot::devices::TPG362::setGaugeOnOff(unsigned short channel, bool onoff)
{
    
    auto st = str_from_number(static_cast<unsigned short>(onoff));
    auto ret = semantic_cmd(channel,parameter_no::sensEnable,action::describe,&st);
    auto interpret = interpret_response_telegram(ret);
    validate_response_telegram_parameters(channel,parameter_no::sensEnable,interpret);
    
}

bool foxtrot::devices::TPG362::getDegass(unsigned short channel)
{
    auto ret = semantic_cmd(channel,parameter_no::degas,action::read);
    auto interpret = interpret_response_telegram(ret);
    validate_response_telegram_parameters(channel,parameter_no::degas,interpret);
    
    return static_cast<bool>(std::stoi(std::get<2>(interpret)));
    
}

void foxtrot::devices::TPG362::setDegass(unsigned short channel, bool onoff)
{
    auto st = str_from_number(static_cast<short unsigned>(onoff));
    
    auto ret=  semantic_cmd(channel,parameter_no::degas,action::describe,&st);
    auto interpret = interpret_response_telegram(ret);
    validate_response_telegram_parameters(channel,parameter_no::degas,interpret);
    
}




const string foxtrot::devices::TPG362::getDeviceTypeName() const
{
  return "TPG362";
}


string foxtrot::devices::TPG362::calculate_checksum(string::const_iterator start, string::const_iterator end)
{
    auto checksum = std::accumulate(start, end, 0, 
    [] (const char& c1, const char& c2)
    {
      return static_cast<int>(c1) + static_cast<int>(c2);
    }
  );

    return str_from_number(checksum % 256,3);
}



std::string foxtrot::devices::TPG362::calculate_checksum(const std::string& message)
{
  
  return calculate_checksum(message.begin(),message.end());
  
}


std::string foxtrot::devices::TPG362::cmd(const std::string& request)
{
  
//   std::cout << "request is: " << request << std::endl;

  std::ostringstream oss;
  

  oss << request << calculate_checksum(request) << '\r';
  
//   std::cout <<"request with checksum is: " << oss.str() << std::endl;

  _serproto->write(oss.str());
  
  //HACK: need a better way to wait for the reply
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
  auto repl = _serproto->read_until_endl('\r');
  return repl;
  
}

string foxtrot::devices::TPG362::semantic_cmd(short unsigned channel, parameter_no p, action readwrite, const std::string* data)
  {
    std::ostringstream oss;
    //checksum and CR get put in by this->cmd 
    
    oss <<str_from_number(static_cast<short unsigned>(_address),2) << 
    str_from_number(channel,1) << 
    str_from_number(static_cast<short unsigned>(readwrite),2) << 
    str_from_number(static_cast<short unsigned>(p),3);
    
    switch(readwrite)
    {
      case action::read : 
	 oss << "02=?";
	 break;
      case action::describe:
	if(data == nullptr)
	{
	  throw std::logic_error("semantic_cmd got a nullptr for data");
	}
	oss << str_from_number(data->size(),2) << *data;
    };
    
    return cmd(oss.str());
  
  }


string foxtrot::devices::TPG362::nmemonic_cmd(short unsigned int channel, const string& request)
{
  //TODO: implement
  throw std::logic_error("mnemonic commands not supported yet");
  

}

  
  
std::tuple<int,int,string> foxtrot::devices::TPG362::interpret_response_telegram(const string& response)
{
  //calculate a checksum: the last 4 characters are 3 char checksum + carriage return

  auto csum_calc = calculate_checksum(response.begin(), response.end() - 4);
//   std::cout << "calculated response checksum is: " << csum_calc << std::endl;
  _lg.strm(sl::debug) << "calculated checksum is: " << csum_calc;

  //validate checksum
  if(response.compare(response.size()-4,3,csum_calc)  != 0)
    {
      throw DeviceError("got invalid checksum!");
    };

  
  //validate the '10' characters
  if(response.compare(3,2,"10") != 0)
  {
    throw DeviceError("unexpected sequence in response telegram");
  };

  //extract parameters from response
  auto addr = std::stoi(response.substr(0,3));
  auto paramno = std::stoi(response.substr(5,3));
  auto dlen = std::stoi(response.substr(8,2));
  
  auto data = response.substr(10,dlen);
 

  return std::make_tuple(addr,paramno,data);

}

void foxtrot::devices::TPG362::validate_response_telegram_parameters(int channel, parameter_no p, const std::tuple< int, int, string >& resp)
{
  if(std::get<0>(resp) != (_address * 10 + channel))
  {
    throw DeviceError("mismatched device address in reply");
    
  }
  if(std::get<1>(resp) != (static_cast<short unsigned int>(p)))
  {
   throw DeviceError("mismatched parameter number in reply");
  }
    
  if(std::get<2>(resp).compare(0,6,"NO_DEF") == 0)
  {
    throw DeviceError("the parameter number does not exist");
  }
  else if (std::get<2>(resp).compare(0,6,"_RANGE") == 0)
  {
    throw DeviceError("the parameter is out of range");
  }
  else if (std::get<2>(resp).compare(0,6,"_LOGIC") == 0)
  {
    throw DeviceError("Logical Access Violation");
  }
  
  

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
