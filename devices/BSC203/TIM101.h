#pragma once

#include <vector>
#include <array>

#include <rttr/registration>

#include <foxtrot/Logging.h>
#include <foxtrot/DeviceError.h>

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/server/Device.h>

#include "APT.h"

//TODO: handle error conditions somehow!!!!

namespace foxtrot {
  namespace devices {
      
    enum class jogdir : unsigned char 
    {
        forward = 0x01,
        reverse = 0x02
    };
    
    #pragma pack(push,1)
    struct jogparams 
    {
        unsigned short subMsgID;
        unsigned short chanIndent;
        unsigned short jogMode;
        unsigned int jogStepSize;
        unsigned int jogStepRate;
        unsigned int jogStepAccn;
    };
    
    #pragma pack(push,1)
   struct motor_status{
       channel_status channel1;
       channel_status channel2;
       channel_status channel3;
       channel_status channel4;
   };
        
    #pragma pack(pop)
    
    class TIM101 : public APT
    {
    public:
        TIM101(std::shared_ptr< protocols::SerialPort > proto);
        void absolute_move(destination dest, motor_channel_idents channel, unsigned distance);
        void jog_move(destination dest, motor_channel_idents channel, jogdir direction);
        void set_jog_parameters(destination dest, jogparams* jogstructp);
        jogparams request_jog_parameters(destination dest);
        motor_status get_status_update(destination dest);
        
    protected:
        void start_update_messages(destination dest);
        void stop_update_messages(destination dest);
        bool check_code_serport(bsc203_opcodes expected_opcode);
    };
    
    //Free functions
    void print_motor_status(motor_status* motorstr);
    void print_channel_status(channel_status* chanstr);
    
  }//namespace devices
} //namespace foxtrot


