#include <foxtrot/DeviceError.h>
#include "BSC203.h"


#define DEST_HOST_CONTROLLER 0x01
#define DEST_RACK_CONTROLLER 0x11
#define DEST_BAY_1 0x21
#define DEST_BAY_2 0x22
#define DEST_BAY_3 0x23
#define DEST_GENERIC_USB_HW_UNIT 0x50

#ifdef linux
#include <byteswap.h>
#endif

#include <thread>
#include <chrono>

using std::cout;
using std::endl;

foxtrot::devices::BSC203::BSC203(std::shared_ptr< foxtrot::protocols::SerialPort > proto) : foxtrot::devices::APT(proto)
{
      cout << "BSC203 Calling superclass constructor..." << endl;
      
      //send this random magical message that makes stuff work for some reason
      _lg.Debug("disabling flash programming on rack...");
      transmit_message(bsc203_opcodes::MGMSG_HW_NO_FLASH_PROGRAMMING,0,0,destination::rack);
    
      for(unsigned char i =0 ; i <3; i++)
      {
        if(get_bayused_rack(destination::rack, i))
        {
            _lg.Debug("bay is used: " + std::to_string(i));
            _lg.Debug("disabling flash programming on controller " + std::to_string(i));
            transmit_message(bsc203_opcodes::MGMSG_HW_NO_FLASH_PROGRAMMING,0,0,
                            static_cast<destination>(i + 0x20));
            
        }
        else
        {
            _lg.Debug("bay is unused: " + std::to_string(i));
        };

      };
    
    //disable status update messages as they will mess with out synchronous messaging model
    _lg.Debug("stopping update messages...");
    transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::rack);
    transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::bay1);
    transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::bay2);
    transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::bay3);
    _lg.Debug("update messages stopped");
 
}



bool foxtrot::devices::BSC203::get_bayused_rack(foxtrot::devices::destination dest, unsigned char bay)
{
    transmit_message(bsc203_opcodes::MGMSG_RACK_REQ_BAYUSED,bay,0,dest);
    auto ret = receive_message_sync(bsc203_opcodes::MGMSG_RACK_GET_BAYUSED,dest);
    
    if(ret.p1 != bay)
    {
      _lg.Error("requested bay: " + std::to_string(bay) + " got bay: " + std::to_string(ret.p1));
      throw DeviceError("invalid bay returned...");
    }
    
    bool used = (ret.p2 == 0x02) ? false : true;
    
    return used;
};

template<typename T>
std::array<unsigned char, 6> get_move_request_header_data(T distance, foxtrot::devices::motor_channel_idents chan)
{
    unsigned char* distbytes = reinterpret_cast<unsigned char*>(&distance);
    std::array<unsigned char, 6> data{static_cast<unsigned char>(chan), 0, distbytes[0], distbytes[1], distbytes[2], distbytes[3]};
    
    return data;
}

void foxtrot::devices::BSC203::relative_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan, int distance)
{
    auto data = get_move_request_header_data(distance, chan);
    auto out = request_response_struct<channel_status>(bsc203_opcodes::MGMSG_MOT_MOVE_RELATIVE,
                                                     bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED,
                                                     dest, data);
    //TODO: check contents of channel_status struct
}


void foxtrot::devices::BSC203::absolute_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan, unsigned distance)
{
    auto data = get_move_request_header_data(distance, chan);
    auto out = request_response_struct<channel_status>(bsc203_opcodes::MGMSG_MOT_MOVE_ABSOLUTE,
                                                     bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED,
                                                     dest, data);
    
    //TODO: check contents of motor status struct
}



