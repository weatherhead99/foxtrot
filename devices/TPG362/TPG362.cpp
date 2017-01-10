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


double foxtrot::devices::TPG362::getPressure(short unsigned int channel)
{
  
  auto ret = semantic_cmd(channel,parameter_no::Pressure,action::read);
  
  std::cout << "got response: " << ret << std::endl;
  
  return 0.;

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
  std::cout << "request is: " << request << std::endl;

  std::ostringstream oss;
  

  oss << request << calculate_checksum(request) << '\r';
  
  std::cout <<"request with checksum is: " << oss.str() << std::endl;

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


string foxtrot::devices::TPG362::interpret_response_telegram(const string& response)
{
  //calculate a checksum and check it. The last 4 chars are checksum and CR
  auto csum_calc = calculate_checksum(response.begin(), response.end() - 4);
  std::cout << "calculated response checksum is: " << csum_calc << std::endl;
  

}


  