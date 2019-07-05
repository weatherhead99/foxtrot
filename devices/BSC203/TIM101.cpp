#include <foxtrot/DeviceError.h>
#include "TIM101.h"


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

foxtrot::devices::TIM101::TIM101(std::shared_ptr< foxtrot::protocols::SerialPort > proto) : foxtrot::devices::APT(proto)
{
 cout << "TIM101 Calling superclass constructor..." << endl;
}

template<typename T>
std::array<unsigned char, 6> get_move_request_header_data(T distance, foxtrot::devices::motor_channel_idents chan)
{
    unsigned char* distbytes = reinterpret_cast<unsigned char*>(&distance);
    std::array<unsigned char, 6> data{static_cast<unsigned char>(chan), 0, distbytes[0], distbytes[1], distbytes[2], distbytes[3]};
    
    return data;
}

void foxtrot::devices::TIM101::absolute_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan, unsigned distance)
{
    auto data = get_move_request_header_data(distance, chan);
    auto out = request_response_struct<motor_status>(bsc203_opcodes::MGMSG_PZMOT_MOVE_ABSOLUTE,
                                                     bsc203_opcodes::MGMSG_PZMOT_MOVE_COMPLETED,
                                                     dest, data);
    
    //TODO: check contents of motor status struct
}

void foxtrot::devices::TIM101::jog_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan, foxtrot::devices::jogdir direction)
{
    auto out = request_response_struct<motor_status>(bsc203_opcodes::MGMSG_PZMOT_MOVE_JOG,bsc203_opcodes::MGMSG_PZMOT_MOVE_COMPLETED,dest,static_cast<unsigned char>(chan), static_cast<unsigned char>(direction));
}
    

