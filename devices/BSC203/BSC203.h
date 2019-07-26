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
   struct velocity_params
   {
       unsigned short chan_indent;
       unsigned int minvel;
       unsigned int acceleration;
       unsigned int maxvel;
   }; 
   
    #pragma pack(push,1)
   struct move_relative_params{
       unsigned short chanIndent;
       unsigned int rel_distance;
   };
   
    #pragma pack(push,1)
   struct position_counter{
       unsigned short chanindent;
       unsigned int position;
   };
   
    #pragma pack (push,1)
   struct jogparamsBSC{
        unsigned short chanIndent;
        unsigned short jogMode;
        unsigned int jogStepSize;
        unsigned int jogMinVel;
        unsigned int jogAccn;
        unsigned int jogMaxVel;
        unsigned short jogStopMode;
   };
   
    #pragma pack (push,1)
   struct homeparams{
       unsigned short chanIndent;
       unsigned short homeDir; //1->UP; 2->DOWN.
       unsigned short limitSwitch;
       unsigned int homeVelocity;
       unsigned int offsetDistance;
   };
   
    #pragma pack (push,1)
   struct powerparams{
       unsigned short chanIndent;
       unsigned short restFactor;
       unsigned short moveFactor;
   };
   
   enum class channelID: unsigned char {
       channel_1 = 0x01,
       channel_2 = 0x02,
       channel_3 = 0x03,
   };
   
   #pragma pack (push,1)
   struct limitswitchparams{
       unsigned short chan_indent;
       unsigned short CWhard;
       unsigned short CCWhard;
       unsigned int CWsoft;
       unsigned int CCWsoft;
       unsigned short limitMode;
   };
   
   #pragma pack (push,1)
   struct PMDjoystickparams{
       unsigned short chan_indent;
       unsigned int lowMaxVel;
       unsigned int highMaxVel;
       unsigned int highLowAccn;
       unsigned int highHighAccn;
       unsigned short dirSense;
   };
    
    #pragma pack(pop)
    
    class BSC203 : public APT
    {
        RTTR_ENABLE()
    public:
        BSC203(std::shared_ptr< protocols::SerialPort > proto);
	const std::string getDeviceTypeName() const override;
	
        void identify_module (destination rackdest, channelID idchan);
        void set_channelenable(destination dest, motor_channel_idents channel, bool onoff);
        void set_limit_switch_params(destination dest, motor_channel_idents channel, const limitswitchparams& limitstr);
        void set_bowindex(destination dest, motor_channel_idents channel, int bowindex);
        void require_status_update(destination dest);
        void require_digoutputs(destination dest);
        void relative_move(destination dest, motor_channel_idents channel, int distance);
        void absolute_move(destination dest, motor_channel_idents channel, unsigned distance);
        bool get_bayused_rack(destination dest, unsigned char bay);
        void jog_move(destination dest, motor_channel_idents channel, unsigned char direction);
        channel_status get_status_update (destination dest, bool print = true);
        
        void set_velocity_params (destination dest, velocity_params* velpar);
        void get_velocity_params(destination dest);
        
        void set_relative_move_params(destination dest, motor_channel_idents channel, int distance);
        void set_absolute_move_params(destination dest, motor_channel_idents channel, 
                                      int distance);
        void set_PMD_params(destination dest, motor_channel_idents channel, const PMDjoystickparams& PMDjoystickstr);
        void get_relative_move_params(destination dest);
        void set_jog_params(destination dest, motor_channel_idents channel, const jogparamsBSC& jogstr);
        void get_jog_params(destination dest);
        
        void set_poscounter(destination dest, motor_channel_idents channel, int position);
        void get_poscounter(destination dest);
        
        void set_enccounter(destination dest, motor_channel_idents channel, int enccount);
        void get_enccounter(destination dest);
        
        void set_homeparams(destination dest, motor_channel_idents channel,const homeparams& homestr);
        void get_homeparams(destination dest);
        
        void set_power_parameters(destination dest, motor_channel_idents channel, const powerparams& powerstr); //Not working on BSC203 controller, the power is computed as a function of the velocity
        void get_power_parameters(destination dest);
        
        void set_generalmove_params(destination dest, motor_channel_idents channel, int backlashdis);
        void get_generalmove_params(destination dest);
        
        void homing_channel(destination dest, motor_channel_idents chan);
        
    protected:
        void start_update_messages(destination dest);
        void stop_update_messages(destination dest);
        bool check_code_serport(bsc203_opcodes expected_opcode);
    };
    
    void print_channel_status(channel_status* chanstr);
    
  }//namespace devices
} //namespace foxtrot

