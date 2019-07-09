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
    
    #pragma pack(pop)
    
    class BSC203 : public APT
    {
    public:
        BSC203(std::shared_ptr< protocols::SerialPort > proto);
        void relative_move(destination dest, motor_channel_idents channel, int distance);
        void absolute_move(destination dest, motor_channel_idents channel, unsigned distance);
        bool get_bayused_rack(destination dest, unsigned char bay);
    };
    
  }//namespace devices
} //namespace foxtrot

