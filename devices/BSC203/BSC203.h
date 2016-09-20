#pragma once

#include "SerialPort.h"
#include "Device.h"

#include <vector>
#include <array>

//TODO: handle error conditions somehow!!!!

namespace foxtrot {
  namespace devices {
    
    enum class destination : unsigned char
    {
     host = 0x01,
     rack = 0x11,
     bay1 = 0x21,
     bay2 = 0x22,
     bay3 = 0x23
    };
    
    class BSC203 : public Device
    {
    public:
    BSC203(std::shared_ptr< protocols::SerialPort > proto);
    
    void identify_module(destination dest);
      
    protected:
      void transmit_message(unsigned short opcode, unsigned char p1, unsigned char p2, destination dest,
			    destination src = destination::host);
      
      template<typename arrtp>
      void transmit_message(unsigned short opcode, arrtp& data, destination dest, 
			    destination src = destination::host);
    
      
      
    private:
      std::shared_ptr<protocols::SerialPort> _serport;
    };
    
    
    
  }//namespace devices
} //namespace foxtrot


template<typename arrtp>
void foxtrot::devices::BSC203::transmit_message(short unsigned int opcode, arrtp& data, destination  dest, 
						destination src)
{ 
  unsigned char* len = &data.size(); 
  unsigned char* optpr = reinterpret_cast<unsigned char*>(&opcode);
  std::array<unsigned char, 6> header{ optpr[1], optpr[0], len[1],len[0], static_cast<unsigned char>(dest) ,static_cast<unsigned char>(src)};
  
  _serport->write(std::string(header.begin(), header.end()));
  _serport->write(std::string(data.begin(), data.end()));

}
