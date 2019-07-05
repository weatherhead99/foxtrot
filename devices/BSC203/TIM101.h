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
        
    #pragma pack(pop)
    
    class TIM101 : public APT
    {
    public:
        TIM101(std::shared_ptr< protocols::SerialPort > proto);
        void absolute_move(destination dest, motor_channel_idents channel, unsigned distance);
        void jog_move(destination dest, motor_channel_idents channel, jogdir direction);
    };
    
  }//namespace devices
} //namespace foxtrot


