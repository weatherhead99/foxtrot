#pragma once

#include <vector>
#include <array>

#include <rttr/registration>

#include <foxtrot/Logging.h>
#include <foxtrot/DeviceError.h>

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/server/Device.h>


//TODO: handle error conditions somehow!!!!

using std::cout;
using std::endl;

namespace foxtrot {
  namespace devices {
      
    enum class destination : unsigned char
    {
        host = 0x01,
        rack = 0x11,
        sourceTIM101 = 0x50,
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
      MGMSG_PZMOT_MOVE_COMPLETED = 0x08D6,
      
      MGMSG_MOT_MOVE_ABSOLUTE = 0x0453,
      MGMSG_PZMOT_MOVE_ABSOLUTE = 0x04D8,
      
      MGMSG_MOT_MOVE_JOG = 0x046A,
      MGMSG_PZMOT_MOVE_JOG = 0x8D9,
      
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
      MGMSG_MOT_GET_STATUSBITS = 0x042A,
      
      MGMSG_PZMOT_SET_PARAMS = 0x08C0,
      MGMSG_PZMOT_REQ_PARAMS = 0x08C1,
      MGMSG_PZMOT_GET_PARAMS = 0x08C2,
      
      MGMSG_HW_START_UPDATEMSGS = 0x0011,
      MGMSG_HW_STOP_UPDATEMSGS = 0x0012,
      MGMSG_PZMOT_GET_STATUSUPDATE = 0x8E1

    };
    
    struct bsc203_reply
    {
      unsigned char p1;
      unsigned char p2;
      std::vector<unsigned char> data;      
    };
    
#pragma pack(push,1)
    struct hwinfo
    {
      unsigned int serno;
      unsigned long modelno;
      unsigned short type;
      unsigned int fwvers;
      std::array<char,48> notes;
      std::array<char,12> emptyspace;
      unsigned short HWvers;
      unsigned short modstate;
      unsigned short nchans;
    };
    
#pragma pack(push,1)
   struct channel_status
   {
       unsigned short chan_indent;
       unsigned int position;
       unsigned int enccount;
       unsigned int statusbits;
   }; 

   
#pragma pack(pop)
    
    class APT : public Device
    {
        
    RTTR_ENABLE()
        
    public:
    void identify_module(destination dest);
    void set_channelenable(destination dest, motor_channel_idents channel, bool onoff);
    bool get_channelenable(destination dest, motor_channel_idents channel);
    
    hwinfo get_hwinfo(destination dest);
      
    bool get_bayused_rack(destination dest, unsigned char bay);
    
    void home_channel(destination dest, motor_channel_idents channel);
    
    void relative_move(destination dest, motor_channel_idents channel, int distance);
    void absolute_move(destination dest, motor_channel_idents channel, unsigned distance);

    protected:
      APT(std::shared_ptr< protocols::SerialPort > proto);
      void transmit_message(bsc203_opcodes opcode, unsigned char p1, unsigned char p2, destination dest, destination src = destination::host);
      
      template<typename arrtp>
      void transmit_message(bsc203_opcodes opcode, arrtp& data, destination dest, destination src = destination::host);
      
      bsc203_reply receive_message_sync(bsc203_opcodes expected_opcode, destination expected_source,
          bool* has_data = nullptr, bool check_opcode = true, unsigned* received_opcode = nullptr);
      
      template<typename T>
      T request_response_struct(bsc203_opcodes opcode_send, bsc203_opcodes opcode_recv, destination dest, unsigned char p1, unsigned char p2);
    
      template<typename T, typename arrtp>
      T request_response_struct(bsc203_opcodes opcode_send, bsc203_opcodes opcode_recv, destination dest, arrtp& data);
      
      foxtrot::Logging _lg;
      
      std::shared_ptr<protocols::SerialPort> _serport;
    
    };
    
    void printhwinfo(hwinfo infostr);
    
  }//namespace devices
} //namespace foxtrot





template<typename arrtp>
void foxtrot::devices::APT::transmit_message(foxtrot::devices::bsc203_opcodes opcode, arrtp& data, destination  dest, destination src)
{ 

  auto size = data.size();
  unsigned char* len = reinterpret_cast<unsigned char*>(&size); 
  unsigned char* optpr = reinterpret_cast<unsigned char*>(&opcode);
  unsigned char destaddr = static_cast<unsigned char>(dest) | 0x80;
  unsigned char srcaddr = static_cast<unsigned char>(src);
  
  std::array<unsigned char, 6> header{optpr[0], optpr[1], len[0],len[1], destaddr, srcaddr};
  
  _serport->write(std::string(header.begin(), header.end()));
  _serport->write(std::string(data.begin(), data.end()));

}

template<typename T>
T foxtrot::devices::APT::request_response_struct(foxtrot::devices::bsc203_opcodes opcode_send, 
                                                    foxtrot::devices::bsc203_opcodes opcode_recv, 
                                                    destination dest, unsigned char p1, unsigned char p2)
{
    T out;
    transmit_message(opcode_send, p1, p2, dest);
    bool has_data;
    auto ret = receive_message_sync(opcode_recv,dest, &has_data);
    
    if(!has_data)
        throw DeviceError("expected struct data in response but didn't get any!");
    
    if(ret.data.size() != sizeof(T))
        throw std::logic_error("mismatch between received data size and struct size!");
    
    std::copy(ret.data.begin(), ret.data.end(), reinterpret_cast<unsigned char*>(&out));
    
    return out;
    
}

template<typename T, typename arrtp>
T foxtrot::devices::APT::request_response_struct(bsc203_opcodes opcode_send, bsc203_opcodes opcode_recv, destination dest, arrtp& data)
{
    T out;
    transmit_message(opcode_send, data, dest);
    _lg.Trace("after transmit message");
    bool has_data;
    auto ret = receive_message_sync(opcode_recv, dest, &has_data);
    _lg.Trace("after received message");
    if(!has_data)
        throw DeviceError("expected struct data in response but didn't get any!");
    
    if(ret.data.size() != sizeof(T))
        throw std::logic_error("mismatch between receieved data size and struct size!");
    
    std::copy(ret.data.begin(), ret.data.end(), reinterpret_cast<unsigned char*>(&out));
    return out;
}

