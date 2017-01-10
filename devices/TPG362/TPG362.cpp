#include "TPG362.h"

#include "DeviceError.h"
#include <SerialPort.h>
#include <iostream>

#include <numeric>
#include <iomanip>

#include <chrono>
#include <thread>

foxtrot::devices::TPG362::TPG362(std::shared_ptr< foxtrot::SerialProtocol > proto)
: CmdDevice(proto), _serproto(proto)
{
  
  auto specproto = std::dynamic_pointer_cast<foxtrot::protocols::SerialPort>(_serproto);
  if(specproto != nullptr)
  {
    std::cout << "using serial connected gauge controller" << std::endl;
    _serialmode = true;
    _serproto->Init(nullptr);
    
  }
  else
  {
    throw foxtrot::DeviceError("ethernet not supported... yet");
    _serialmode = false;
  };
  
  
    
}

double foxtrot::devices::TPG362::getPressure_ch1()
{
  
  auto ret = semantic_cmd(parameter_no::Pressure,action::read,nullptr);
  

}

double foxtrot::devices::TPG362::getPressure_ch2()
{

}




std::string foxtrot::devices::TPG362::calculate_checksum(const std::string& message)
{
  auto checksum = std::accumulate(message.begin(), message.end(), 0, 
    [] (const char& c1, const char& c2)
    {
      return static_cast<int>(c1) + static_cast<int>(c2);
    }
  );
  
  return str_from_number(checksum % 256,3);
  
}


std::string foxtrot::devices::TPG362::cmd(const std::string& request)
{
  std::ostringstream oss;
  
  oss << request << calculate_checksum(request) << '\r';
  
  _serproto->write(oss.str());
  
  //HACK: need a better way to wait for the reply
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
  auto repl = _serproto->read_until_endl('\r');
  return repl;
  
}

string foxtrot::devices::TPG362::semantic_cmd(parameter_no p, action readwrite, const std::string* data)
  {
    std::ostringstream oss;
    //checksum and CR get put in by this->cmd 
    
    oss <<str_from_number(static_cast<short unsigned>(_address),3) << 
    str_from_number(static_cast<short unsigned>(readwrite),2) << 
    str_from_number(static_cast<short unsigned>(p),3);
    
    switch(readwrite)
    {
      case action::read : 
	 oss << "02=?";
      case action::describe:
	if(data == nullptr)
	{
	  throw std::logic_error("semantic_cmd got a nullptr for data");
	}
	oss << str_from_number(data->size(),2) << *data;
    };
    
    return cmd(oss.str());
  
  }

