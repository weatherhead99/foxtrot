#pragma once
#include "APT.h"


//TODO: handle error conditions somehow!!!!

namespace foxtrot {
  namespace devices {
      
   #pragma pack(push,1)

   
   struct move_relative_params{
       unsigned short chanIndent;
       unsigned int rel_distance;
   };
   
   struct position_counter{
       unsigned short chanindent;
       unsigned int position;
   };
   
   struct jogparamsBSC{
        unsigned short chanIndent;
        unsigned short jogMode;
        unsigned int jogStepSize;
        unsigned int jogMinVel;
        unsigned int jogAccn;
        unsigned int jogMaxVel;
        unsigned short jogStopMode;
   };
   

   
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
        void set_channelenable(destination dest, motor_channel_idents channel, bool onoff) override;
        void set_limit_switch_params(destination dest, motor_channel_idents channel, const limitswitchparams& limitstr);
        void set_bowindex(destination dest, motor_channel_idents channel, int bowindex);
        void require_status_update(destination dest);
        void require_digoutputs(destination dest);
        void relative_move(destination dest, motor_channel_idents channel, int distance);
        void absolute_move(destination dest, motor_channel_idents channel, unsigned distance);
        bool get_bayused_rack(destination dest, unsigned char bay);
        void jog_move(destination dest, motor_channel_idents channel, unsigned char direction);
        channel_status get_status_update (destination dest, bool print = true);
        
        
        void set_relative_move_params(destination dest, motor_channel_idents channel, int distance);
        void set_absolute_move_params(destination dest, motor_channel_idents channel, 
                                      int distance);
        void set_PMD_params(destination dest, motor_channel_idents channel, const PMDjoystickparams& PMDjoystickstr);
        move_relative_params get_relative_move_params(destination dest);
        void set_jog_params(destination dest, motor_channel_idents channel, const jogparamsBSC& jogstr);
        jogparamsBSC get_jog_params(destination dest);
        
        void set_poscounter(destination dest, motor_channel_idents channel, int position);
        unsigned int get_poscounter(destination dest);
        
        void set_enccounter(destination dest, motor_channel_idents channel, int enccount);
        unsigned int get_enccounter(destination dest);
        
        void set_homeparams(destination dest, motor_channel_idents channel,const homeparams& homestr);
        homeparams get_homeparams(destination dest);
        
        void set_power_parameters(destination dest, motor_channel_idents channel, const powerparams& powerstr); //Not working on BSC203 controller, the power is computed as a function of the velocity
        powerparams get_power_parameters(destination dest);
        
        void set_generalmove_params(destination dest, motor_channel_idents channel, int backlashdis);
        unsigned int get_generalmove_params(destination dest);
        
        void homing_channel(destination dest, motor_channel_idents chan);
        
    protected:
        bool check_code_serport(bsc203_opcodes expected_opcode);
    };
    
    void print_channel_status(channel_status* chanstr);
    
  }//namespace devices
} //namespace foxtrot

