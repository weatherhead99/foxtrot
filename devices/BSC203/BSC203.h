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
      
      MGMSG_MOD_REQ_HWINFO = 0x0005,
      MGMSG_MOD_GET_HWINFO = 0x0006,
      
      MGMSG_HW_NO_FLASH_PROGRAMMING = 0x0018,
      
      MGMSG_RACK_REQ_BAYUSED = 0x0060,
      MGMSG_RACK_GET_BAYUSED = 0x0061,
      
      MGMSG_RACK_REQ_STATUSBITS = 0x0226,
      MGMSG_RACK_GET_STATUSBITS = 0x0227,
      
      MGMSG_RACK_SET_DIGOUTPUTS = 0x0228,
      MGMSG_RACK_REQ_DIGOUTPUTS = 0x0229,
      MGMSG_RACK_GET_DIGOUTPUTS = 0x0230,
      
      MGMSG_MOT_SET_POSCOUNTER = 0x0410,
      MGMSG_MOT_REQ_POSCOUNTER = 0x0411,
      MGMSG_MOT_GET_POSCOUNTER = 0x0412,
      
      MGMSG_MOT_SET_ENCCOUNTER = 0x0409,
      MGMSG_MOT_REQ_ENCCOUNTER = 0x040A,
      MGMSG_MOT_GET_ENCCOUNTER = 0x040B,
      
      MGMSG_MOT_SET_VELPARAMS = 0x0413,
      MGMSG_MOT_REQ_VELPARAMS = 0x0414,
      MGMSG_MOT_GET_VELPARAMS = 0x0415,
      
      MGMSG_MOT_SET_JOGPARAMS = 0x0416,
      MGMSG_MOT_REQ_JOGPARAMS = 0x0417,
      MGMSG_MOT_GET_JOGPARAMS = 0x0418,
      
      MGMSG_MOT_REQ_ADCINPUTS = 0x042B,
      MGMSG_MOT_GET_ADCINPUTS = 0x042C,
      
      MGMSG_MOT_SET_POWERPARAMS = 0x0426,
      MGMSG_MOT_REQ_POWERPARAMS = 0x0427,
      MGMSG_MOT_GET_POWERPARAMS = 0x0428,
      
      MGMSG_MOT_SET_GENMOVEPARAMS = 0x043A,
      MGMSG_MOT_REQ_GENMOVEPARAMS = 0x043B,
      MGMSG_MOT_GET_GENMOVEPARAMS = 0x043C,
      
      MGMSG_MOT_SET_MOVERELPARAMS = 0x0445,
      MGMSG_MOT_REQ_MOVERELPARAMS = 0x0446,
      MGMSG_MOT_GET_MOVERELPARAMS = 0x0447,
      
      MGMSG_MOT_SET_MOVEABSPARAMS = 0x0450,
      MGMSG_MOT_REQ_MOVEABSPARAMS = 0x0451,
      MGMSG_MOT_GET_MOVEABSPARAMS = 0x0452,
      
      MGMSG_MOT_SET_HOMEPARAMS = 0x0440,
      MGMSG_MOT_REQ_HOMEPARAMS = 0x0441,
      MGMSG_MOT_GET_HOMEPARAMS = 0x0442,
      
      MGMSG_MOT_SET_LIMSWITCHPARAMS = 0x0423,
      MGMSG_MOT_REQ_LIMSWITCHPARAMS = 0x0424,
      MGMSG_MOT_GET_LIMSWITCHPARAMS = 0x0425,
      
      MGMSG_MOT_MOVE_HOME = 0x0443,
      MGMSG_MOT_MOVE_HOMED = 0x0444,
      
      MGMSG_MOT_MOVE_RELATIVE = 0x0448,
      
      MGMSG_MOT_MOVE_COMPLETED = 0x0464,
      
      MGMSG_MOT_MOVE_ABSOLUTE = 0x0453,
      
      MGMSG_MOT_MOVE_JOG = 0x046A,
      
      MGMSG_MOT_MOVE_VELOCITY = 0x0457,
      
      MGMSG_MOT_MOVE_STOP = 0x0465,
      MGMSG_MOT_MOVE_STOPPED = 0x0466,
      
      MGMSG_MOT_SET_BOWINDEX = 0x04F4,
      MGMSG_MOT_REQ_BOWINDEX = 0x04F5,
      MGMSG_MOT_GET_BOWINDEX = 0x04F6,
      
      MGMSG_MOT_SET_DCPIDPARAMS = 0x04A0,
      MGMSG_MOT_REQ_DCPIDPARAMS = 0x04A1,
      MGMSG_MOT_GET_DCPIDPARAMS = 0x04A2,
      
      MGMSG_MOT_GET_STATUSUPDATE = 0x0481,
      MGMSG_MOT_REQ_STATUSUPDATE = 0x0480,
      
      MGMSG_MOT_REQ_DCSTATUSUPDATE = 0x0490,
      MGMSG_MOT_GET_DCSTATUSUPDATE = 0x0491,
      
      MGMSG_MOT_REQ_STATUSBITS = 0x0429,
      MGMSG_MOT_GET_STATUSBITS = 0x042A
      
      
      
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
      
    bool get_bayused_rack(destination dest, unsigned char bay);
    
    void home_channel(destination dest, motor_channel_idents channel);
    
    
    void relative_move(destination dest, motor_channel_idents channel, unsigned distance);
    void absolute_move(destination dest, motor_channel_idents channel, unsigned distance);
    
    
    
    protected:
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
