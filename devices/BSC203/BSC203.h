#pragma once

#include "SerialPort.h"
#include "Device.h"

#include <vector>
#include <array>

#include "Logging.h"

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
    
    enum class motor_channel_idents : unsigned char{
        channel_1 = 0x01,
        channel_2 = 0x02,
        channel_3 = 0x04,
        channel_4 = 0x08
    };
        
    
    
    enum class bsc203_opcodes : unsigned short {
      MGMSG_MOD_IDENTIFY = 0x0223,
      MGMSG_MOD_SET_CHANENABLESTATE = 0x0210,
      MGMSG_MOD_REQ_CHANENABLESTATE = 0x0211,
      MGMSG_MOD_GET_CHANENABLESTATE = 0x0212,
      MGMSG_MOD_START_UPDATEMSGS = 0x0011,
      MGMSG_MOD_STOP_UPDATEMSGS = 0x0012,
      MGMSG_MOD_GET_HWINFO = 0x0006
    };
    
    struct bsc203_reply
    {
      unsigned char p1;
      unsigned char p2;
      std::vector<unsigned char> data;      
    };
    
    
    struct hwinfo
    {
      unsigned int serno;
      unsigned long modelno;
      unsigned short type;
      unsigned int fwvers;
      std::array<char,48> notes;
      unsigned short HWvers;
      unsigned short modstate;
      unsigned short nchans;
    };
    
    
    class BSC203 : public Device
    {
    public:
    BSC203(std::shared_ptr< protocols::SerialPort > proto);
    
    void identify_module(destination dest);
    void set_channelenable(destination dest, motor_channel_idents channel, bool onoff);
    bool get_channelenable(destination dest, motor_channel_idents channel);
    
    hwinfo get_hwinfo(destination dest);
      
    protected:
      void set_updatemsgs(destination dest, bool onoff);
        
      void transmit_message(bsc203_opcodes opcode, unsigned char p1, unsigned char p2, destination dest,
			    destination src = destination::host);
      
      template<typename arrtp>
      void transmit_message(bsc203_opcodes opcode, arrtp& data, destination dest, 
			    destination src = destination::host);
    
      
      bsc203_reply receive_message_sync(bsc203_opcodes expected_opcode, destination expected_source,
          bool* has_data = nullptr);
      
      foxtrot::Logging _lg;
      
    private:
      std::shared_ptr<protocols::SerialPort> _serport;
    };
    
    
    
  }//namespace devices
} //namespace foxtrot


template<typename arrtp>
void foxtrot::devices::BSC203::transmit_message(foxtrot::devices::bsc203_opcodes opcode, arrtp& data, destination  dest, destination src)
{ 
  unsigned char* len = &data.size(); 
  unsigned char* optpr = reinterpret_cast<unsigned char*>(&opcode);
  std::array<unsigned char, 6> header{ optpr[1], optpr[0], len[1],len[0], static_cast<unsigned char>(dest) |0x80 ,static_cast<unsigned char>(src)};
  
  _serport->write(std::string(header.begin(), header.end()));
  _serport->write(std::string(data.begin(), data.end()));

}
