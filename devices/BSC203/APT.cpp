#include <foxtrot/DeviceError.h>
#include "APT.h"


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
#include <string>

using std::cout;
using std::endl;

const foxtrot::parameterset bsc203_class_params
{
  {"baudrate", 115200u},
  {"stopbits", 1u},
  {"flowcontrol", "hardware"},
  {"parity", "none"}
  
  
};


foxtrot::devices::APT::APT(std::shared_ptr< foxtrot::protocols::SerialPort > proto) 
: foxtrot::Device(proto), _serport(proto), _lg("BSC203")
{
  _serport->Init(&bsc203_class_params);
  _serport->setDrain(true);
  _serport->flush();

}


void foxtrot::devices::APT::transmit_message(foxtrot::devices::bsc203_opcodes opcode, unsigned char p1, unsigned char p2, destination dest, destination src)
{
  auto optpr = reinterpret_cast<unsigned char*>(&opcode);
  
  std::array<unsigned char, 6> header{ optpr[0], optpr[1], p1, p2, static_cast<unsigned char>(dest) ,static_cast<unsigned char>(src)};
  
  _lg.Trace("writing to serial port...");
  _serport->write(std::string(header.begin(), header.end()));
  
}

foxtrot::devices::bsc203_reply foxtrot::devices::APT::receive_message_sync(foxtrot::devices::bsc203_opcodes expected_opcode, foxtrot::devices::destination expected_source, bool* has_data, bool check_opcode, unsigned* received_opcode)
{
    unsigned actlen;
    bsc203_reply out;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // IT SEEMS TO WORK FINE WITH 50 ms, PLEASE DO NOT CHANGE IT.
    
    //cout << " Bytes available: " << std::hex << _serport->bytes_available() << endl;
    
    auto headerstr = _serport->read(6,&actlen);
    
    while(headerstr[0] == 0){
        if (_serport->bytes_available() == 0){
            break;
        }
        auto extra_byte = _serport->read(1);
        std::copy(headerstr.begin() + 1, headerstr.end() + 1, headerstr.begin());
    }
    
    if(actlen != 6)
    {
        _lg.Error("bad reply length: " + std::to_string(actlen));
        throw DeviceError("received bad reply length..." );
    };
    
    unsigned opcode = ( static_cast<unsigned>(headerstr[1]) <<8 ) | static_cast<unsigned>(headerstr[0] & 0xFF); 
    //std::cout << std::hex << (unsigned) opcode << std::endl;
    
    if (received_opcode != nullptr)
    {
        *received_opcode = opcode;
    }

    if(opcode != static_cast<decltype(opcode)>(expected_opcode))
    {
        _lg.Error("unexpected opcode: " + std::to_string(opcode));
        _lg.Error("expected: " + std::to_string(static_cast<decltype(opcode)>(expected_opcode)));
        
        if (check_opcode)
        {
            throw DeviceError("received unexpected opcode");
        }
    }

    if (headerstr[4] == 0)
    {
        _lg.Error("null destination");
        throw DeviceError("received null destination");
    }

    auto src = headerstr[5];
    if(src != static_cast<decltype(src)>(expected_source))
    {
        _lg.Error("unexpected source: " + std::to_string(src) + ", expected: " +  std::to_string(static_cast<decltype(src)>(expected_source)));
        throw DeviceError("received unexpected source");

    }
    
    //check if data packet present 
    if( (headerstr[4] & 0x80) == 0x80)
    {
        //WARNING: IS THE DLEN MSB OR LSB?
      _lg.strm(sl::trace) << "headerstr[3]: " << (int) headerstr[3] << " headerstr[2]: " << (int) headerstr[2];
      //      unsigned short dlen = (static_cast<int>(headerstr[3]) << 8 ) & static_cast<int>(headerstr[2]);
      unsigned short dlen = (static_cast<unsigned short>(headerstr[3]) << 8) | static_cast<unsigned short>(headerstr[2]);  
      
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


bool foxtrot::devices::APT::get_channelenable(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel)
{
    transmit_message(bsc203_opcodes::MGMSG_MOD_REQ_CHANENABLESTATE,static_cast<unsigned char>(channel),0, dest);
    
    auto ret = receive_message_sync(bsc203_opcodes::MGMSG_MOD_GET_CHANENABLESTATE,dest);
    
    bool onoff = (ret.p2 == 0x01) ? true : false;
    
    return onoff;
}


foxtrot::devices::hwinfo foxtrot::devices::APT::get_hwinfo(foxtrot::devices::destination dest,
							   std::optional<destination> expd_src)
{   
    auto out = request_response_struct<hwinfo>(bsc203_opcodes::MGMSG_MOD_REQ_HWINFO,
                                               bsc203_opcodes::MGMSG_MOD_GET_HWINFO, dest,0x00, 0x00,
					       expd_src);

    return out;
};


void foxtrot::devices::APT::home_channel(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan)
{
    transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_HOMED,static_cast<unsigned char>(chan),0,dest);

    auto ret = receive_message_sync(bsc203_opcodes::MGMSG_MOT_MOVE_HOMED,dest);
    
    if(ret.p1 != static_cast<unsigned char>(chan))
    {
        throw DeviceError("invalid channel returned...");
    }
    
    

};

template<typename T>
std::array<unsigned char, 6> get_move_request_header_data(T distance, foxtrot::devices::motor_channel_idents chan)
{
    unsigned char* distbytes = reinterpret_cast<unsigned char*>(&distance);
    std::array<unsigned char, 6> data{static_cast<unsigned char>(chan), 0, distbytes[0], distbytes[1], distbytes[2], distbytes[3]};
    return data;
}



void foxtrot::devices::printhwinfo(foxtrot::devices::hwinfo infostr)
{
    cout << "Printing hwinfo..." << endl;
    cout << "\tSerial number: " << std::dec << infostr.serno << endl;
    cout << "\tType: " << std::dec << infostr.type << endl;
    cout << "\t Model number: ";
    for(int i=0; i< 8; i++)
      cout << (char) infostr.modelno[i];
    cout << endl;
    cout << "\tFirmware Version: " << std::dec << infostr.fwvers << endl;
    cout << "\tNotes: ";
    for (int i = 0; i < 47; i++)
    {
        cout << infostr.notes[i];
    }
    cout << endl;
    cout << "\tHW Version: " << std::dec <<infostr.HWvers << endl;
    cout << "\tMod State: " << std::dec << infostr.modstate << endl;
    cout << "\tNumber of channels: " << std::dec << infostr.nchans << endl;
    
}

RTTR_REGISTRATION{
    using namespace rttr;
    using foxtrot::devices::APT;
    registration::class_<APT>("foxtrot::devices::APT")
    
    .method("get_channelenable", &APT::get_channelenable)
    (parameter_names("destination", "channel"))
    .method("get_hwinfo", &APT::get_hwinfo)
      (parameter_names("destination", "expd_src"))
    .method("home_channel", &APT::home_channel)
    (parameter_names("destination", "channel"));
    
    
    //Custom structs
    using foxtrot::devices::bsc203_reply;
    registration::class_<bsc203_reply>("foxtrot::devices::bsc203_reply")
    .constructor()(policy::ctor::as_object)
    .property("p1", &bsc203_reply::p1)
    .property("p2", &bsc203_reply::p2)
    .property("data", &bsc203_reply::data);
    
    using foxtrot::devices::hwinfo;
    registration::class_<hwinfo>("foxtrot::devices::hwinfo")
    .constructor()(policy::ctor::as_object)
    .property("serno", &hwinfo::serno)
    .property("modelno", &hwinfo::modelno)
    .property("type", &hwinfo::type)
    .property("fwvers", &hwinfo::fwvers)
    .property("notes", &hwinfo::notes)
    .property("emptyspace", &hwinfo::emptyspace)
    .property("HWvers", &hwinfo::HWvers)
    .property("modstate", &hwinfo::modstate)
    .property("nchans", &hwinfo::nchans);
    
    using foxtrot::devices::channel_status;
    registration::class_<channel_status>("foxtrot::devices::channel_status")
    .constructor()(policy::ctor::as_object)
    .property("chan_indent", &channel_status::chan_indent)
    .property("position", &channel_status::position)
    .property("enccount", &channel_status::enccount)
    .property("statusbits", &channel_status::statusbits);
    
    //Custom enums
    using foxtrot::devices::destination;
    registration::enumeration<destination>("foxtrot::devices::destination")
    (value("host", destination::host),
     value("rack", destination::rack),
     value("sourceTIM101", destination::sourceTIM101),
     value("bay1", destination::bay1),
     value("bay2", destination::bay2),
     value("bay3", destination::bay3));
    
    using foxtrot::devices::motor_channel_idents;
    registration::enumeration<motor_channel_idents>("foxtrot::devices::motor_channel_idents")
    (value("channel_1", motor_channel_idents::channel_1),
     value("channel_2", motor_channel_idents::channel_2),
     value("channel_3", motor_channel_idents::channel_3),
     value("channel_4", motor_channel_idents::channel_4));
}



