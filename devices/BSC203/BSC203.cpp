#include "BSC203.h"
#include <DeviceError.h>


#define DEST_HOST_CONTROLLER 0x01
#define DEST_RACK_CONTROLLER 0x11
#define DEST_BAY_1 0x21
#define DEST_BAY_2 0x22
#define DEST_BAY_3 0x23
#define DEST_GENERIC_USB_HW_UNIT 0x50


#include <byteswap.h>

#include <thread>
#include <chrono>


const foxtrot::parameterset bsc203_class_params
{
  {"baudrate", 115200u},
  {"stopbits", 1u},
  {"flowcontrol", "hardware"},
  {"parity", "none"}
  
  
};


foxtrot::devices::BSC203::BSC203(std::shared_ptr< foxtrot::protocols::SerialPort > proto) 
: foxtrot::Device(proto), _serport(proto), _lg("BSC203")
{
  
  _serport->Init(&bsc203_class_params);
  
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
  
  _lg.Debug("stopping update messages...");
  //disable status update messages as they will mess with out synchronous messaging model
  transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::rack);
  transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::bay1);
  transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::bay2);
  transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::bay3);
  
  _lg.Debug("update messages stopped");
  
}


void foxtrot::devices::BSC203::identify_module(foxtrot::devices::destination dest)
{
    //no reply expected
  transmit_message(bsc203_opcodes::MGMSG_MOD_IDENTIFY,0,0,dest);
}


void foxtrot::devices::BSC203::transmit_message( foxtrot::devices::bsc203_opcodes opcode, unsigned char p1, unsigned char p2, destination dest, destination src)
{
  
  auto optpr = reinterpret_cast<unsigned char*>(&opcode);
  
  std::array<unsigned char, 6> header{ optpr[0], optpr[1], p1, p2, static_cast<unsigned char>(dest) ,static_cast<unsigned char>(src)};
  
  _lg.Trace("writing to serial port...");
  _serport->write(std::string(header.begin(), header.end()));
  

}

foxtrot::devices::bsc203_reply foxtrot::devices::BSC203::receive_message_sync(foxtrot::devices::bsc203_opcodes expected_opcode, foxtrot::devices::destination expected_source, bool* has_data)
{
    unsigned actlen;
    bsc203_reply out;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    
    auto headerstr = _serport->read(6,&actlen);
    
    if(actlen != 6)
    {
        _lg.Error("bad reply length: " + std::to_string(actlen));
        throw DeviceError("received bad reply length..." );
    };
    
//     std::cout << "headerstr len: " << headerstr.size() << std::endl;
    
    for(auto& c : headerstr)
    {
      std::cout << std::hex << (unsigned) c << "\t";
    }
    std::cout << std::endl;
    
    
    unsigned opcode = ( static_cast<unsigned>(headerstr[1]) <<8 ) | static_cast<unsigned>(headerstr[0]); 
    std::cout << (unsigned ) opcode << std::endl;
    
    
    if(opcode != static_cast<decltype(opcode)>(expected_opcode))
    {
        _lg.Error("unexpected opcode: " + std::to_string(opcode));
	_lg.Error("expected: " + std::to_string(static_cast<decltype(opcode)>(expected_opcode)));
        throw DeviceError("received unexpected opcode");
    }
    
    auto src = headerstr[5];
    if(src != static_cast<decltype(src)>(expected_source))
    {
        _lg.Error("unexpected source: " + std::to_string(src));
        throw DeviceError("received unexpected source");
    }
    
    //check if data packet present 
    if( (headerstr[4] & 0x80) == 0x80)
    {
        //WARNING: IS THE DLEN MSB OR LSB?
        unsigned short dlen = (headerstr[3] << 8 ) & headerstr[2];  
        
      _lg.Trace("data packet present, length: " + std::to_string(dlen));
      
      auto data = _serport->read(dlen,&actlen);
      if(actlen != dlen)
      {
          _lg.Error("didn't read all the data..." + std::to_string(actlen));
          throw DeviceError("unexpected data length!");
      }
      
      out.data = std::vector<unsigned char>(data.begin(),data.end());
      
      if(has_data != nullptr)
      {
          *has_data = true;
      }
    }
    else
    {
        if(has_data != nullptr)
        {
            *has_data = false;
        }
    }
    
    out.p1 = headerstr[2];
    out.p2 = headerstr[3];
      
      
    return out;
    
}


void foxtrot::devices::BSC203::set_channelenable(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, bool onoff)
{
    unsigned char enable_disable = onoff? 0x01 : 0x02;
    
    
    transmit_message(bsc203_opcodes::MGMSG_MOD_SET_CHANENABLESTATE,static_cast<unsigned char>(channel),
                     enable_disable, dest);
}

bool foxtrot::devices::BSC203::get_channelenable(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel)
{
    transmit_message(bsc203_opcodes::MGMSG_MOD_REQ_CHANENABLESTATE
    ,static_cast<unsigned char>(channel),0, dest);
    
    auto ret = receive_message_sync(bsc203_opcodes::MGMSG_MOD_GET_CHANENABLESTATE,dest);
    
    bool onoff = (ret.p2 == 0x01) ? true : false;
    
    return onoff;
}



foxtrot::devices::hwinfo foxtrot::devices::BSC203::get_hwinfo(foxtrot::devices::destination dest)
{
    hwinfo out;
    
    transmit_message(bsc203_opcodes::MGMSG_MOD_REQ_HWINFO,0x00,0x00,dest);
    auto ret = receive_message_sync(bsc203_opcodes::MGMSG_MOD_GET_HWINFO,dest);
    
    //TODO: fill in hwinfo struct sensibly
    
    return out;
};

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


void foxtrot::devices::BSC203::home_channel(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan)
{
    transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_HOME,static_cast<unsigned char>(chan),0,dest);
    
    auto ret = receive_message_sync(bsc203_opcodes::MGMSG_MOT_MOVE_HOMED,dest);
    
    if(ret.p1 != static_cast<unsigned char>(chan))
    {
        throw DeviceError("invalid channel returned...");
    }
    
    
};


void foxtrot::devices::BSC203::relative_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan, unsigned distance)
{
    std::array<unsigned char,6> data {static_cast<unsigned char>(chan),0,0,0,0,0};
    
//      transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_RELATIVE,
    
}

void foxtrot::devices::BSC203::absolute_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan, unsigned distance)
{
    
    
}






