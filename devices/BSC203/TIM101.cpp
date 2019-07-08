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
static std::array<unsigned char, 18> get_jog_set_request_data(foxtrot::devices::jogparams* jogstruct);



foxtrot::devices::TIM101::TIM101(std::shared_ptr< foxtrot::protocols::SerialPort > proto) : foxtrot::devices::APT(proto)
{
    _lg.strm(sl::trace) <<"TIM101 Calling superclass constructor...";
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
    
    //Setting channel to th active channel
    set_channelenable(dest, chan, true);
    
    auto data = get_move_request_header_data(distance, chan);
    auto out = request_response_struct<motor_status>(bsc203_opcodes::MGMSG_PZMOT_MOVE_ABSOLUTE,
                                                     bsc203_opcodes::MGMSG_PZMOT_MOVE_COMPLETED,
                                                     dest, data);
    
    //TODO: check contents of motor status struct
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

    //The stop_update_messages of the constructor blocks the serial port. For that reason we need to turn it on again.
    start_update_messages(dest);

    //Waiting for MOVE COMPLETE HEADER
    while(!check_code_serport(bsc203_opcodes::MGMSG_PZMOT_MOVE_COMPLETED))
    {
        //_lg.strm(sl::trace) << "looking for move complete...";
    }
    
    _serport->flush();
    
    //Extracting position information from GET STATUS UPDATE
    while(received_opcode != static_cast<decltype(received_opcode)>(bsc203_opcodes::MGMSG_PZMOT_GET_STATUSUPDATE))
    {
        try{
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

        //Printing motor_status information
        print_motor_status(&motorstr);
    

        } catch (DeviceError excep){
            //_lg.strm(sl::trace) << "looking for get status...";
        }
        
    }

    stop_update_messages(dest);

}
    
void foxtrot::devices::TIM101::set_jog_parameters(foxtrot::devices::destination dest, foxtrot::devices::jogparams* jogstruct){
    
    auto data = get_jog_set_request_data(jogstruct);
    
    transmit_message(bsc203_opcodes::MGMSG_PZMOT_SET_PARAMS, data, dest);

    
    
}

foxtrot::devices::jogparams foxtrot::devices::TIM101::request_jog_parameters(foxtrot::devices::destination dest){

    auto out =request_response_struct<jogparams>(bsc203_opcodes::MGMSG_PZMOT_REQ_PARAMS, bsc203_opcodes::MGMSG_PZMOT_GET_PARAMS, dest, 0x09,0x01);
    
    return out;
    
}
    
void foxtrot::devices::TIM101::start_update_messages(foxtrot::devices::destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_HW_START_UPDATEMSGS, 0x0A,0x0, dest);
    
}

void foxtrot::devices::TIM101::stop_update_messages(foxtrot::devices::destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_HW_STOP_UPDATEMSGS, 0x0,0x0, dest);
    
}

foxtrot::devices::motor_status foxtrot::devices::TIM101::get_status_update(foxtrot::devices::destination dest)
{
    
    auto out = request_response_struct<motor_status>(bsc203_opcodes::MGMSG_HW_START_UPDATEMSGS, bsc203_opcodes::MGMSG_PZMOT_GET_STATUSUPDATE, dest, 0x0A,0x0);
    
    stop_update_messages(dest);
    
    return out;
    
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
static std::array<unsigned char, 18> get_jog_set_request_data(foxtrot::devices::jogparams* jogstructp)
{
    unsigned char* subMsgbytes = reinterpret_cast<unsigned char*>(&jogstructp->subMsgID);
    unsigned char* subJogModebytes = reinterpret_cast<unsigned char*>(&jogstructp->jogMode);
    unsigned char* subJogStepSizebytes = reinterpret_cast<unsigned char*>(&jogstructp->jogStepSize);
    unsigned char* subJogStepRatebytes = reinterpret_cast<unsigned char*>(&jogstructp->jogStepRate);
    unsigned char* subJogStepAccnbytes = reinterpret_cast<unsigned char*>(&jogstructp->jogStepAccn);
    
   std::array<unsigned char, 18> data{subMsgbytes[0], subMsgbytes[1], static_cast<unsigned char>(jogstructp->chanIndent), 0, subJogModebytes[0], subJogModebytes[1], subJogStepSizebytes[0], subJogStepSizebytes[1], subJogStepSizebytes[2], subJogStepSizebytes[3], subJogStepRatebytes[0], subJogStepRatebytes[1], subJogStepRatebytes[2], subJogStepRatebytes[3], subJogStepAccnbytes[0], subJogStepAccnbytes[1], subJogStepAccnbytes[2], subJogStepAccnbytes[3]};

    return data;
}
    

void foxtrot::devices::print_motor_status(foxtrot::devices::motor_status* motorstr){
    
    cout << "Motor Status: Position Chanel 1 (hex): "<< std::hex << motorstr->channel1.position<< endl;
    cout << "Motor Status: Position Chanel 2 (hex): "<< std::hex << motorstr->channel2.position << endl;
    cout << "Motor Status: Position Chanel 3 (hex): "<< std::hex << motorstr->channel3.position << endl;
    cout << "Motor Status: Position Chanel 4 (hex): "<< std::hex << motorstr->channel4.position << endl;
}

void foxtrot::devices::print_channel_status(foxtrot::devices::channel_status* chanstr){
    
    cout << "Chanel Code (hex): "<< std::hex << chanstr->chan_indent << endl;
    cout << "Absolute Position (hex): "<< std::hex << chanstr->position << endl;

}
    
    
    
    
