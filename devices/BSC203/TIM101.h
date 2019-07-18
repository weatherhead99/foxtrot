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
      
      
#pragma pack(push,1)
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
    
    #pragma, pack(push,1)
    struct move_absolute_params
    {
        unsigned short subMsgID;
        unsigned short chanIndent;
        unsigned short maxVoltage;
        unsigned int stepRate;
        unsigned int stepAccn;
    };
    
    #pragma, pack(push,1)
    struct pos_counter_params
    {
        unsigned short subMsgID;
        unsigned short chanIndent;
        unsigned int position;
        unsigned int encCount;
    };
    
    #pragma pack(push,1)
    struct motor_status{
       channel_status channel1;
       channel_status channel2;
       channel_status channel3;
       channel_status channel4;
    };
   
   /*struct motor_status{
       std::array<channel_status,4> channelstat;
   };*/
        
    #pragma pack(pop)
    
    class TIM101 : public APT
    {
    public:
        TIM101(std::shared_ptr< protocols::SerialPort > proto);
        void identify_module(destination dest);
        void set_channelenable(destination dest, motor_channel_idents channel, bool onoff);
        void absolute_move(destination dest, motor_channel_idents channel, int distance);
        void set_move_absolute_parameters(destination dest, move_absolute_params* absparams);
        void request_move_absolute_parameters(destination dest);
        void jog_move(destination dest, motor_channel_idents channel, jogdir direction);
        void set_jog_parameters(destination dest, jogparams* jogstructp);
        void request_jog_parameters(destination dest);
        void get_status_update(destination dest, bool print = true);
        void set_position_counter(destination dest, pos_counter_params* absparams);
        void request_position_counter(destination dest);
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


