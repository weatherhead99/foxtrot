#pragma once
#include <vector>
#include <array>
#include <optional>

namespace foxtrot {
    namespace devices {
                
    enum class destination : unsigned char
    {
        host = 0x01,
        rack = 0x11,
        genericUSB = 0x50,
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

      enum class status_bit_indices : unsigned char {
	is_homing = 9
	


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
      MGMSG_PZMOT_MOVE_ABSOLUTE = 0x08D4,
      
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
      MGMSG_MOT_ACK_STATUSUPDATE = 0x0492,
      
      MGMSG_MOT_REQ_STATUSBITS = 0x0429,
      MGMSG_MOT_GET_STATUSBITS = 0x042A,
      
      MGMSG_PZMOT_SET_PARAMS = 0x08C0,
      MGMSG_PZMOT_REQ_PARAMS = 0x08C1,
      MGMSG_PZMOT_GET_PARAMS = 0x08C2,
      
      MGMSG_HW_START_UPDATEMSGS = 0x0011,
      MGMSG_HW_STOP_UPDATEMSGS = 0x0012,
      MGMSG_PZMOT_GET_STATUSUPDATE = 0x8E1,
      
      MGMSG_MOD_SET_DIGOUTPUTS = 0x0213,
      MGMSG_MOD_REQ_DIGOUTPUTS = 0x0214,
      MGMSG_MOD_GET_DIGOUTPUTS = 0x0215,
      MGMSG_MOT_SET_PMDJOYSTICKPARAMS = 0x04E6,

      
      MGMSG_MOT_SUSPEND_ENDOFMOVEMSGS = 0x046B,
      MGMSG_MOT_RESUME_ENDOFMOVEMSGS = 0x046C,

      MGMSG_HW_RICHRESPONSE = 0x0081,
      MGMSG_HW_RESPONSE = 0x0080
      
    };

    
    struct bsc203_reply
    {
      unsigned char p1;
      unsigned char p2;
      std::vector<unsigned char> data;
    };

    struct apt_reply
    {
      unsigned char p1;
      unsigned char p2;
      std::optional<std::vector<unsigned char>> data = std::nullopt;
    };

#pragma pack(push, 1)
    struct hwinfo
    {
      unsigned int serno;
      std::array<char, 8> modelno;
      //      unsigned long modelno;
      unsigned short type;
      unsigned int fwvers;
      std::array<char,48> notes;
      std::array<char,12> emptyspace;
      unsigned short HWvers;
      unsigned short modstate;
      unsigned short nchans;
    };
    
   struct channel_status
   {
       unsigned short chan_ident;
       unsigned int position;
       unsigned int enccount;
       unsigned int statusbits;
   }; 

   struct dcstatus 
   {
       unsigned short chan_ident;
       unsigned int position;
       unsigned short velocity;
       unsigned short motorcurrent;
       unsigned int statusbits;
   };


   struct velocity_params
   {
       unsigned short chan_ident;
       unsigned int minvel;
       unsigned int acceleration;
       unsigned int maxvel;
   }; 


    struct homeparams{
       unsigned short chan_ident;
       unsigned short homeDir; //1->UP; 2->DOWN.
       unsigned short limitSwitch;
       unsigned int homeVelocity;
       unsigned int offsetDistance;
   };
   
    struct limitswitchparams{
       unsigned short chan_ident;
       unsigned short CWhard;
       unsigned short CCWhard;
       unsigned int CWsoft;
       unsigned int CCWsoft;
       unsigned short limitMode;
   };
   

#pragma pack(pop)
    

    }
}
