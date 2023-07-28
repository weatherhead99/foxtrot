#pragma once

#include <vector>
#include <array>

#include <rttr/registration>

#include <foxtrot/Logging.h>
#include <foxtrot/DeviceError.h>

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Device.h>

#include "APT.h"
#include "APT_defs.hh"

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
        unsigned short chanIdent;
        unsigned short jogMode;
        unsigned int jogStepSize;
        unsigned int jogStepRate;
        unsigned int jogStepAccn;
    };


    #pragma pack(push,1)
    struct piezo_move_absolute_params
    {
    unsigned short subMsgID;
    unsigned short chanIdent;
    unsigned short maxVoltage;
    unsigned int stepRate;
    unsigned int stepAccn;
    };

    #pragma, pack(push,1)
    struct pos_counter_params
    {
        unsigned short subMsgID;
        unsigned short chanIdent;
        int position;
        unsigned int encCount;
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
        RTTR_ENABLE(APT)
    public:
        TIM101(std::shared_ptr< protocols::SerialPort > proto);
	const std::string getDeviceTypeName() const override;
        void identify_module();
        void absolute_move(motor_channel_idents channel, int distance);
        void set_move_absolute_parameters(const piezo_move_absolute_params& absparams);
        piezo_move_absolute_params request_move_absolute_parameters(motor_channel_idents chan);
        void jog_move(motor_channel_idents channel, jogdir direction);
        void set_jog_parameters(const jogparams& jogstructp);
        jogparams request_jog_parameters(motor_channel_idents channel);
        motor_status get_status_update();
        
        pos_counter_params position_counter(motor_channel_idents channel);
        void set_position_counter(motor_channel_idents channel, int position);
        
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


