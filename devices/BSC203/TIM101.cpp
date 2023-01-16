#include <foxtrot/DeviceError.h>
#include <foxtrot/protocols/SerialPort.h>
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

//Static functions
static std::array<unsigned char, 18> get_jog_set_request_data(const foxtrot::devices::jogparams& jogstruct);
static std::array<unsigned char, 14> get_move_absolute_request_data(const foxtrot::devices::move_absolute_params& absparams);
static std::array<unsigned char, 14> get_pos_counter_request_data(const foxtrot::devices::pos_counter_params& poscountparams);


foxtrot::devices::TIM101::TIM101(std::shared_ptr< foxtrot::protocols::SerialPort > proto) : foxtrot::devices::APT(proto)
{
    _lg.strm(sl::trace) <<"TIM101 Calling superclass constructor...";

    //send this random magical message that makes stuff work for some reason
    _lg.Debug("disabling flash programming on rack...");
    transmit_message(bsc203_opcodes::MGMSG_HW_NO_FLASH_PROGRAMMING,0,0,destination::genericUSB);

    //disable status update messages as they will mess with out synchronous messaging model
    _lg.Debug("stopping update messages...");
    transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::genericUSB);
    _lg.Debug("update messages stopped");
    
    
    foxtrot::devices::motor_channel_idents channels[4] = {foxtrot::devices::motor_channel_idents::channel_1, foxtrot::devices::motor_channel_idents::channel_2,         foxtrot::devices::motor_channel_idents::channel_3, foxtrot::devices::motor_channel_idents::channel_4};
    
    //Initialize movement parameters
    jogparams jogstruct;
    jogstruct.subMsgID = 0x0009;
    jogstruct.chanIndent = 0x01;
    jogstruct.jogMode = 0x0002; //step mode
    jogstruct.jogStepSize = 250;//Can be positive or negative
    jogstruct.jogStepRate = 0x000001F4;
    jogstruct.jogStepAccn = 0x000186A0;

    move_absolute_params absparams;
    absparams.subMsgID = 0x0007;    absparams.chanIndent = 0x01;
    absparams.maxVoltage = 0x006E;
    absparams.stepRate = 0x000000AA;
    absparams.stepAccn = 0x000000F0;
    
    _lg.Debug("Initializing jog parameters");
    _lg.Debug("Initializing move absolute parameters");
    
    for (auto c:channels)
    {
        cout << static_cast<short unsigned int>(c) << endl;
        jogstruct.chanIndent = static_cast<short unsigned int>(c);
        set_jog_parameters(destination::genericUSB, jogstruct);
        absparams.chanIndent = static_cast<short unsigned int>(c);
        set_move_absolute_parameters(destination::genericUSB, absparams);
    }
    
}

const std::string foxtrot::devices::TIM101::getDeviceTypeName() const
{
  return "TIM101";
}

void foxtrot::devices::TIM101::identify_module(foxtrot::devices::destination dest)
{
    //no reply expected
    transmit_message(bsc203_opcodes::MGMSG_MOD_IDENTIFY,0,0,dest);
}


void foxtrot::devices::TIM101::set_channelenable(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, bool onoff)
{
    unsigned char enable_disable = onoff? 0x01 : 0x02;

    transmit_message(bsc203_opcodes::MGMSG_MOD_SET_CHANENABLESTATE,static_cast<unsigned char>(channel),enable_disable, dest);

}

template<typename T>
std::array<unsigned char, 6> get_move_request_header_data(T distance, foxtrot::devices::motor_channel_idents chan)
{
    unsigned char* distbytes = reinterpret_cast<unsigned char*>(&distance);
    std::array<unsigned char, 6> data {static_cast<unsigned char>(chan), 0, distbytes[0], distbytes[1], distbytes[2], distbytes[3]};

    return data;
}

void foxtrot::devices::TIM101::absolute_move(foxtrot::devices::motor_channel_idents chan, int distance)
{

    auto dest = foxtrot::devices::destination::genericUSB;
    //Setting channel to th active channel
    set_channelenable(dest, chan, true);

    //Checking that it is not already in the same position
    motor_status motorinit = get_status_update();
    switch (chan) {
        case foxtrot::devices::motor_channel_idents::channel_1:
            if (motorinit.channel1.position == distance) {
                return;
            }
        case foxtrot::devices::motor_channel_idents::channel_2:
            if (motorinit.channel2.position == distance) {
                return;
            }
        case foxtrot::devices::motor_channel_idents::channel_3:
            if (motorinit.channel3.position == distance) {
                return;
            }
        case foxtrot::devices::motor_channel_idents::channel_4:
            if (motorinit.channel4.position == distance) {
                return;
            }
    }

    auto data = get_move_request_header_data(distance, chan);
    transmit_message(bsc203_opcodes::MGMSG_PZMOT_MOVE_ABSOLUTE, data, dest);

    //The stop_update_messages of the constructor blocks the serial port. For that reason we need to turn it on again.
    start_update_messages(dest);

    //Waiting for MOVE COMPLETE HEADER
    while(!check_code_serport(bsc203_opcodes::MGMSG_PZMOT_MOVE_COMPLETED))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //_lg.strm(sl::trace) << "looking for move complete...";
    }

    stop_update_messages(dest);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    _serport->flush();

    motor_status motorstat = get_status_update();

}


void foxtrot::devices::TIM101::jog_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan, foxtrot::devices::jogdir direction)
{

    bool hasdata;
    unsigned received_opcode = 0;
    motor_status motorstr;

    //Setting channel to th active channel
    set_channelenable(dest, chan, true);

    //TIM101 only cares about the active channel, if you address a jog move instruction to a non-active channel, nothing will happen
    transmit_message(bsc203_opcodes::MGMSG_PZMOT_MOVE_JOG, static_cast<unsigned char>(chan), static_cast<unsigned char>(direction), dest);

//     //The stop_update_messages of the constructor blocks the serial port. For that reason we need to turn it on again.
//     start_update_messages(dest);
// 
//     //Waiting for MOVE COMPLETE HEADER
//     while(!check_code_serport(bsc203_opcodes::MGMSG_PZMOT_MOVE_COMPLETED))
//     {
//         //_lg.strm(sl::trace) << "looking for move complete...";
//     }
// 
//     stop_update_messages(dest);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    _serport->flush();

    motor_status motorstat = get_status_update();

}

void foxtrot::devices::TIM101::set_jog_parameters(foxtrot::devices::destination dest, const foxtrot::devices::jogparams& jogstruct) {

    auto data = get_jog_set_request_data(jogstruct);

    transmit_message(bsc203_opcodes::MGMSG_PZMOT_SET_PARAMS, data, dest);

}

void foxtrot::devices::TIM101::set_move_absolute_parameters(foxtrot::devices::destination dest, const foxtrot::devices::move_absolute_params& absparams) {

    auto data = get_move_absolute_request_data(absparams);

    transmit_message(bsc203_opcodes::MGMSG_PZMOT_SET_PARAMS, data, dest);

}


foxtrot::devices::jogparams foxtrot::devices::TIM101::request_jog_parameters(foxtrot::devices::destination dest) {

    auto out =request_response_struct<jogparams>(bsc203_opcodes::MGMSG_PZMOT_REQ_PARAMS, bsc203_opcodes::MGMSG_PZMOT_GET_PARAMS, dest, 0x09,0x01, destination::genericUSB);

    //cout << " Jog parameters: Step size = " << std::hex << out.jogStepSize << endl;
    
    return out;

}

foxtrot::devices::move_absolute_params foxtrot::devices::TIM101::request_move_absolute_parameters(foxtrot::devices::destination dest) {

    auto out =request_response_struct<move_absolute_params>(bsc203_opcodes::MGMSG_PZMOT_REQ_PARAMS, bsc203_opcodes::MGMSG_PZMOT_GET_PARAMS, dest, 0x07,0x01, destination::genericUSB);

    //cout << "Move absolute parameters: Step rate (hex) = " << std::hex << out.stepRate << endl;
    //cout << "Move absolute parameters: Step acceleration (hex) = " << std::hex << out.stepAccn << endl;
    
    return out;

}



foxtrot::devices::pos_counter_params 
foxtrot::devices::TIM101::position_counter(foxtrot::devices::motor_channel_idents channel)
{
    unsigned char chanchar = static_cast<unsigned char>(channel);
    
    auto out = request_response_struct<pos_counter_params>(bsc203_opcodes::MGMSG_PZMOT_REQ_PARAMS,
                                                bsc203_opcodes::MGMSG_PZMOT_GET_PARAMS,
                                                destination::genericUSB, 0x05, chanchar, destination::genericUSB);
    return out;
    
}

void foxtrot::devices::TIM101::set_position_counter(foxtrot::devices::motor_channel_idents 
channel, int position)
{
    pos_counter_params params;
    params.chanIndent = static_cast<unsigned char>(channel);
    params.subMsgID = 0x05;
    params.position = position;
    params.encCount = 0x00;
    
    std::array<unsigned char, sizeof(pos_counter_params)> dat;
    
    unsigned char* paramd = reinterpret_cast<unsigned char*>(&params);
    std::copy(paramd, paramd + sizeof(decltype(params)), dat.begin());
    
    transmit_message(bsc203_opcodes::MGMSG_PZMOT_SET_PARAMS, dat, destination::genericUSB, destination::genericUSB);
    
}



void foxtrot::devices::TIM101::start_update_messages(foxtrot::devices::destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_HW_START_UPDATEMSGS, 0x0A,0x0, dest);

}

void foxtrot::devices::TIM101::stop_update_messages(foxtrot::devices::destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_HW_STOP_UPDATEMSGS, 0x0,0x0, dest);

}

foxtrot::devices::motor_status foxtrot::devices::TIM101::get_status_update()
{
    
    auto dest = foxtrot::devices::destination::genericUSB;
    bool hasdata;
    unsigned received_opcode = 0;
    motor_status motorstr;

    start_update_messages(dest);

    _serport->flush();

    //Extracting position information from GET STATUS UPDATE
    while(received_opcode != static_cast<decltype(received_opcode)>(bsc203_opcodes::MGMSG_PZMOT_GET_STATUSUPDATE))
    {
        try {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto ret = receive_message_sync(bsc203_opcodes::MGMSG_PZMOT_GET_STATUSUPDATE, dest, &hasdata, true, &received_opcode);

            //Checking motor_status information
            if(!hasdata)
            {
                throw DeviceError("expected struct data in response but didn't get any!");
            }

            if(ret.data.size() != sizeof(motorstr))
            {
                throw std::logic_error("mismatch between received data size and struct size!");
            }

            std::copy(ret.data.begin(), ret.data.end(), reinterpret_cast<unsigned char*>(&motorstr));

            bool print=false;
            //Printing motor_status information
            if (print) {
                print_motor_status(&motorstr);
            }

            stop_update_messages(dest);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            _serport->flush();

            return motorstr;


        } catch (DeviceError excep) {
            //_lg.strm(sl::trace) << "looking for get status...";
        }

    }
    
    return motorstr;

}


bool foxtrot::devices::TIM101::check_code_serport(foxtrot::devices::bsc203_opcodes expected_opcode)
{
    unsigned actlen;
    bsc203_reply out;
    bool codein = false;
    unsigned nbytes = _serport->bytes_available();
    unsigned opcode;

    //cout << "Bytes available: " << std::hex << nbytes << endl;

    auto headerstr = _serport->read(nbytes,&actlen);

    if (nbytes < 2 )
    {
        return false;
    }

    for (int i = 0; i < (nbytes - 1); i++)
    {
        opcode = ( static_cast<unsigned>(headerstr[i+1]) <<8 ) | static_cast<unsigned>(headerstr[i] & 0xFF);
        //cout << std::hex << opcode << "\t";

        if (opcode == static_cast<decltype(opcode)>(expected_opcode))
        {
            cout << endl;
            codein = true;
            break;
        }
    }

    //cout << endl;
    return codein;

}


//Static functions
static std::array<unsigned char, 18> get_jog_set_request_data(const foxtrot::devices::jogparams& jogstructp)
{
    unsigned char* subMsgbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&jogstructp.subMsgID));
    unsigned char* subJogModebytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&jogstructp.jogMode));
    unsigned char* subJogStepSizebytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&jogstructp.jogStepSize));
    unsigned char* subJogStepRatebytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&jogstructp.jogStepRate));
    unsigned char* subJogStepAccnbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&jogstructp.jogStepAccn));

    std::array<unsigned char, 18> data {subMsgbytes[0], subMsgbytes[1], static_cast<unsigned char>(jogstructp.chanIndent), 0, subJogModebytes[0], subJogModebytes[1], subJogStepSizebytes[0], subJogStepSizebytes[1], subJogStepSizebytes[2], subJogStepSizebytes[3], subJogStepRatebytes[0], subJogStepRatebytes[1], subJogStepRatebytes[2], subJogStepRatebytes[3], subJogStepAccnbytes[0], subJogStepAccnbytes[1], subJogStepAccnbytes[2], subJogStepAccnbytes[3]};

    return data;
}

static std::array<unsigned char, 14> get_move_absolute_request_data(const foxtrot::devices::move_absolute_params& absparams)
{
    unsigned char* subMsgbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&absparams.subMsgID));
    unsigned char* maxVoltagebytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&absparams.maxVoltage));
    unsigned char* stepRatebytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&absparams.stepRate));
    unsigned char* stepAccnbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&absparams.stepAccn));

    std::array<unsigned char, 14> data {subMsgbytes[0], subMsgbytes[1], static_cast<unsigned char>(absparams.chanIndent), 0, maxVoltagebytes[0], maxVoltagebytes[1], stepRatebytes[0], stepRatebytes[1], stepRatebytes[2], stepRatebytes[3], stepAccnbytes[0], stepAccnbytes[1], stepAccnbytes[2], stepAccnbytes[3]};

    return data;

}

// static std::array<unsigned char, 14> get_pos_counter_request_data(const foxtrot::devices::pos_counter_params& poscountparams)
// {
//     unsigned char* subMsgbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&poscountparams.subMsgID));
//     unsigned char* positionbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&poscountparams.position));
//     unsigned char* enccountbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&poscountparams.encCount));
// 
// 
//     std::array<unsigned char, 14> data {subMsgbytes[0], subMsgbytes[1], static_cast<unsigned char>(poscountparams.chanIndent), 0, positionbytes[0], positionbytes[1], positionbytes[2], positionbytes[3], enccountbytes[0], enccountbytes[1], enccountbytes[2], enccountbytes[3]};
// 
//     return data;
// }


//Free functions
void foxtrot::devices::print_motor_status(foxtrot::devices::motor_status* motorstr) {

    cout << "Motor Status: Position Channel 1 (hex): " << std::hex << motorstr->channel1.position << endl;
    cout << "Motor Status: Position Channel 2 (hex): " << std::hex << motorstr->channel2.position << endl;
    cout << "Motor Status: Position Channel 3 (hex): " << std::hex << motorstr->channel3.position << endl;
    cout << "Motor Status: Position Channel 4 (hex): " << std::hex << motorstr->channel4.position << endl;

    /*for (int i = 0; i < 4; i++)
    {
    cout << "Motor Status: Position Chanel " << std::to_string(i) << " (hex): "<< std::hex << motorstr->channelstat[i].position<< endl;
    }*/
}


void foxtrot::devices::print_channel_status(foxtrot::devices::channel_status* chanstr) {

    //cout << "Chanel Code (hex): "<< std::hex << chanstr->chan_indent << endl;
    cout << "Absolute Position (hex): "<< std::hex << chanstr->position << endl;
    cout << "Chanel EncCount (hex): "<< std::hex << chanstr->chan_indent << endl;
    //cout << "Status Bits (hex): "<< std::hex << chanstr->statusbits << endl;

}

RTTR_REGISTRATION {
    using namespace rttr;
    using foxtrot::devices::TIM101;
    registration::class_<TIM101>("foxtrot::devices::TIM101")

    .method("identify_module", &TIM101::identify_module)
    (parameter_names("destination"))
    .method("set_channelenable", &TIM101::set_channelenable)
    (parameter_names("destination", "channel", "on/off"))
    .method("absolute_move", &TIM101::absolute_move)
    (parameter_names("channel", "position"))
    .method("set_move_absolute_parameters", &TIM101::set_move_absolute_parameters)
    (parameter_names("destination", "move absolute parameters"))
    .method("request_move_absolute_parameters", &TIM101::request_move_absolute_parameters)
    (parameter_names("destination"))
    .method("jog_move", &TIM101::jog_move)
    (parameter_names("destination", "channel", "direction"))
    .method("set_jog_parameters", &TIM101::set_jog_parameters)
    (parameter_names("destination", "jog parameters"))
    .method("request_jog_parameters", &TIM101::request_jog_parameters)
    (parameter_names("destination"))
    .method("get_status_update", &TIM101::get_status_update)
    
    .method("position_counter", &TIM101::position_counter)
    (parameter_names("channel"))
    
    .method("set_position_counter", &TIM101::set_position_counter)
    (parameter_names("channel", "counter_value"));

    //Custom structs
    using foxtrot::devices::jogparams;
    registration::class_<jogparams>("foxtrot::devices::jogparams")
    .constructor()(policy::ctor::as_object)
    .property("subMsgID", &jogparams::subMsgID)
    .property("chanIndent", &jogparams::chanIndent)
    .property("jogMode", &jogparams::jogMode)
    .property("jogStepSize", &jogparams::jogStepSize)
    .property("jogStepRate", &jogparams::jogStepRate)
    .property("jogStepAccn", &jogparams::jogStepAccn);

    using foxtrot::devices::move_absolute_params;
    registration::class_<move_absolute_params>("foxtrot::devices::move_absolute_params")
    .constructor()(policy::ctor::as_object)
    .property("subMsgID", &move_absolute_params::subMsgID)
    .property("chanIndent", &move_absolute_params::chanIndent)
    .property("maxVoltage", &move_absolute_params::maxVoltage)
    .property("stepRate", &move_absolute_params::stepRate)
    .property("stepAccn", &move_absolute_params::stepAccn);

    using foxtrot::devices::pos_counter_params;
    registration::class_<pos_counter_params>("foxtrot::devices::pos_counter_params")
    .constructor()(policy::ctor::as_object)
    .property("subMsgID", &pos_counter_params::subMsgID)
    .property("chanIndent", &pos_counter_params::chanIndent)
    .property("position", &pos_counter_params::position)
    .property("encCount", &pos_counter_params::encCount);

    using foxtrot::devices::motor_status;
    registration::class_<motor_status>("foxtrot::devices::motor_status")
    .constructor()(policy::ctor::as_object)
    .property("channel1", &motor_status::channel1)
    .property("channel2", &motor_status::channel2)
    .property("channel3", &motor_status::channel3)
    .property("channel4", &motor_status::channel4);

    using foxtrot::devices::jogdir;
    registration::enumeration<jogdir>("foxtrot::devices::jogdir")
    (value("forward", jogdir::forward),
    value("reverse", jogdir::reverse));

}


