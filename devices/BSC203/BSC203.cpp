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

static std::array<unsigned char, 14> get_velocityparams_data(const foxtrot::devices::velocity_params& velpar);

static std::array<unsigned char, 22> get_jog_set_request_data(const foxtrot::devices::jogparamsBSC& jogstructp);

static std::array<unsigned char, 14> get_homeparams_data(const foxtrot::devices::homeparams& homeparams);

static std::array<unsigned char, 6> get_powerparams_data ( const foxtrot::devices::powerparams& powerstr);

static std::array<unsigned char, 16> get_limitswitch_params(const foxtrot::devices::limitswitchparams& limitstr);

static std::array< unsigned char, 20> get_PMDjoystick_params(const foxtrot::devices::PMDjoystickparams& PMDjoystickstr);

foxtrot::devices::BSC203::BSC203(std::shared_ptr< foxtrot::protocols::SerialPort > proto) : foxtrot::devices::APT(proto)
{
    _lg.Trace("BSC203 Calling superclass constructor...");

    //Constructor based on Throlabs software initialization of the motors
    get_hwinfo(foxtrot::devices::destination::rack);
    
    for(unsigned char i =0 ; i <3; i++)
    {
        if(get_bayused_rack(destination::rack, i))
        {
            _lg.Debug("bay is used: " + std::to_string(i));
            _lg.Debug("disabling flash programming on controller " + std::to_string(i));

            //Changing from 0x20 to 0x21 to point at the bays...
            transmit_message(bsc203_opcodes::MGMSG_HW_NO_FLASH_PROGRAMMING,0,0,static_cast<destination>(i + 0x21));

        }
        else
        {
            _lg.Debug("bay is unused: " + std::to_string(i));
        };

    };

    require_digoutputs(foxtrot::devices::destination::rack);

    transmit_message(bsc203_opcodes::MGMSG_MOT_REQ_STATUSUPDATE, 0x01, 0, foxtrot::devices::destination::rack);
    transmit_message(bsc203_opcodes::MGMSG_MOT_REQ_STATUSUPDATE, 0x01, 0, foxtrot::devices::destination::bay1);
    transmit_message(bsc203_opcodes::MGMSG_MOT_REQ_STATUSUPDATE, 0x01, 0, foxtrot::devices::destination::bay2);
    transmit_message(bsc203_opcodes::MGMSG_MOT_REQ_STATUSUPDATE, 0x01, 0, foxtrot::devices::destination::bay3);

    //disable status update messages as they will mess with out synchronous messaging model
    _lg.Debug("stopping update messages...");
    transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::rack);
    transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::bay1);
    transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::bay2);
    transmit_message(bsc203_opcodes::MGMSG_MOD_STOP_UPDATEMSGS,0,0,destination::bay3);
    _lg.Debug("update messages stopped");

    //Parameter initialization
    
    //Elements
    foxtrot::devices::destination bays[3] = {foxtrot::devices::destination::bay1, foxtrot::devices::destination::bay2, foxtrot::devices::destination::bay3};

    //Values
    foxtrot::devices::velocity_params velpar;
    velpar.chan_indent = 0x01;
    velpar.minvel = 0;
    //velpar.acceleration = 0x09600100; // Default acceleration
    //velpar.maxvel = 0x1A36E2EB; // Default velocity
    velpar.acceleration = 4506;
    velpar.maxvel = 21987328;
    //velpar.acceleration = 4506*4;
    //velpar.maxvel = 21987328*4;

    foxtrot::devices::powerparams powerstr;
    powerstr.chanIndent = 0x01;
    powerstr.moveFactor = 0x1000;
    powerstr.restFactor = 0x0;

    foxtrot::devices::homeparams homestr;
    homestr.chanIndent = 0x01;
    homestr.homeDir = 0x02;
    homestr.limitSwitch = 0x01;// If homeDir == 1 -> 4, if 2-> 1
    homestr.homeVelocity = 0x014F8B59;
    homestr.offsetDistance = 0x0A000;

    foxtrot::devices::limitswitchparams limitstr;
    limitstr.chan_indent = 0x01;
    limitstr.CWhard = 0x03;
    limitstr.CCWhard = 0x03;
    limitstr.CWsoft = 0x12c000;
    limitstr.CCWsoft = 0x064000;
    limitstr.limitMode = 0x01;

    foxtrot::devices::jogparamsBSC jogstruct;
    jogstruct.chanIndent = 0x01;
    jogstruct.jogMode = 0x02;
    jogstruct.jogStepSize = 0x032000;
    jogstruct.jogMinVel = 0;
    jogstruct.jogAccn = 0x08cd;
    jogstruct.jogMaxVel = 0x014f8b59;
    jogstruct.jogStopMode = 0x02;

    foxtrot::devices::PMDjoystickparams PMDjoystickstr;
    PMDjoystickstr.chan_indent = 0x01;
    PMDjoystickstr.lowMaxVel = 0x029f16b1;
    PMDjoystickstr.highMaxVel = 0x0d1b7176;
    PMDjoystickstr.highLowAccn = 0x2334;
    PMDjoystickstr.highHighAccn = 0xb005;
    PMDjoystickstr.dirSense = 0x01;

    for (auto elem : bays)
    {
        set_channelenable(elem,foxtrot::devices::motor_channel_idents::channel_1, false);

        _lg.Debug("Initializing velocity parameters");
        set_velocity_params(elem, &velpar);

        _lg.Debug("Initializing general move parameters");
        set_generalmove_params(elem, foxtrot::devices::motor_channel_idents::channel_1, 0x1000);

        _lg.Debug("Initializing power parameters");
        set_power_parameters(elem, foxtrot::devices::motor_channel_idents::channel_1, powerstr);

        _lg.Debug("Initializing home parameters");
        set_homeparams(elem, foxtrot::devices::motor_channel_idents::channel_1, homestr);

        _lg.Debug("Initializing bow index");
        set_bowindex(elem, foxtrot::devices::motor_channel_idents::channel_1, 0x0);

        _lg.Debug("Initializing limit switch parameters");
        set_limit_switch_params(elem, foxtrot::devices::motor_channel_idents::channel_1, limitstr);

        _lg.Debug("Initializing jog parameters");
        set_jog_params(elem, foxtrot::devices::motor_channel_idents::channel_1, jogstruct);

        _lg.Debug("Initializing relative move distance");
        set_relative_move_params(elem, foxtrot::devices::motor_channel_idents::channel_1, 0xA000);

        _lg.Debug("Initializing absolute move distance");
        set_absolute_move_params(elem, foxtrot::devices::motor_channel_idents::channel_1, 0);

        _lg.Debug("Initializing PMD joystick parameters");
        set_PMD_params(elem, foxtrot::devices::motor_channel_idents::channel_1, PMDjoystickstr);

        require_status_update(elem);

    }
    
    //Flushing serial port
    _lg.Debug("Flushing serial port");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    _serport->flush();

}


const std::string foxtrot::devices::BSC203::getDeviceTypeName() const
{
  return "BSC203";
};

void foxtrot::devices::BSC203::identify_module (foxtrot::devices::destination rackdest, foxtrot::devices::channelID idchan)
{
    //no reply expected
    transmit_message(bsc203_opcodes::MGMSG_MOD_IDENTIFY,static_cast<unsigned char>(idchan),0,rackdest);

}


void foxtrot::devices::BSC203::require_digoutputs(foxtrot::devices::destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_MOD_REQ_DIGOUTPUTS, 0x01,0x0, dest);

}

void foxtrot::devices::BSC203::set_channelenable(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, bool onoff)
{
    unsigned char enable_disable = onoff? 0x01 : 0x02;

    transmit_message(bsc203_opcodes::MGMSG_MOD_SET_CHANENABLESTATE,static_cast<unsigned char>(channel),enable_disable, dest);

    transmit_message(bsc203_opcodes::MGMSG_MOT_REQ_STATUSUPDATE, 0x01, 0, dest);

}


void foxtrot::devices::BSC203::set_limit_switch_params(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, const foxtrot::devices::limitswitchparams& limitstr)
{
    auto data = get_limitswitch_params(limitstr);

    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_LIMSWITCHPARAMS, data, dest);

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
    std::array<unsigned char, 6> data {static_cast<unsigned char>(chan), 0, distbytes[0], distbytes[1], distbytes[2], distbytes[3]};

    return data;
}

void foxtrot::devices::BSC203::relative_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan, int distance)
{

    auto data = get_move_request_header_data(distance, chan);

    transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_RELATIVE,data,dest);

    //The stop_update_messages of the constructor blocks the serial port. For that reason we need to turn it on again.
    start_update_messages(dest);

    //Waiting for MOVE COMPLETED HEADER
    while(!check_code_serport(bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //_lg.strm(sl::trace) << "looking for move complete...";
    }

    stop_update_messages(dest);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    _serport->flush();
    
    channel_status chanstat = get_status_update(dest);

}

bool foxtrot::devices::BSC203::check_code_serport(foxtrot::devices::bsc203_opcodes expected_opcode)
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
            //cout << endl;
            codein = true;
            break;
        }
    }

    //cout << endl;
    return codein;

}


void foxtrot::devices::BSC203::absolute_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan, unsigned distance)
{

    //Set the channel
    set_channelenable(dest, foxtrot::devices::motor_channel_idents::channel_1,true);
    
    //Checking that it is not already in the same position
    channel_status motorinit = get_status_update(dest, false);
    if (motorinit.position == distance) {
        _lg.Info("The actuator is already in the requested position");
        return;
    }

    auto data = get_move_request_header_data(distance, chan);

    transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_ABSOLUTE,data,dest);

    start_update_messages(dest);

    //Waiting for MOVE COMPLETE HEADER
    while(!check_code_serport(bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //_lg.strm(sl::trace) << "looking for move complete...";
    }

    stop_update_messages(dest);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    _serport->flush(); 
    
    channel_status chanstat = get_status_update(dest);
}

void foxtrot::devices::BSC203::jog_move(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, unsigned char direction)
{
    //Set the channel
    set_channelenable(dest, foxtrot::devices::motor_channel_idents::channel_1,true);

    transmit_message(foxtrot::devices::bsc203_opcodes::MGMSG_MOT_MOVE_JOG, static_cast<unsigned char>(channel), direction, dest);

    start_update_messages(dest);

    //Waiting for MOVE COMPLETE HEADER
    while(!check_code_serport(bsc203_opcodes::MGMSG_MOT_MOVE_COMPLETED))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //_lg.strm(sl::trace) << "looking for move complete...";
    }

    stop_update_messages(dest);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    _serport->flush(); 
    
    channel_status chanstat = get_status_update(dest);
}

foxtrot::devices::channel_status foxtrot::devices::BSC203::get_status_update (foxtrot::devices::destination dest, bool print) {

    bool hasdata;
    unsigned received_opcode = 0;
    channel_status chanstr;

    start_update_messages(dest);
    _serport->flush();
    
    while(received_opcode != static_cast<decltype(received_opcode)>(bsc203_opcodes::MGMSG_MOT_GET_STATUSUPDATE))
    {
        try
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto ret = receive_message_sync(bsc203_opcodes::MGMSG_MOT_GET_STATUSUPDATE, dest, &hasdata, true, &received_opcode);

            std::copy(ret.data.begin(), ret.data.end(), reinterpret_cast<unsigned char*>(&chanstr));
            
            if (print){
                print_channel_status(&chanstr);
            }
            
            stop_update_messages(dest);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            _serport->flush();
            
            return chanstr;

        } catch (DeviceError excep)
        {
            _lg.strm(sl::trace) << "looking for get status...";
        }
    }


}

void foxtrot::devices::BSC203::start_update_messages(foxtrot::devices::destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_HW_START_UPDATEMSGS, 0x0A,0x0, dest);

}

void foxtrot::devices::BSC203::stop_update_messages(foxtrot::devices::destination dest)
{
    transmit_message(bsc203_opcodes::MGMSG_HW_STOP_UPDATEMSGS, 0x0,0x0, dest);

}

void foxtrot::devices::BSC203::homing_channel(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents chan)
{
    transmit_message(bsc203_opcodes::MGMSG_MOT_MOVE_HOME,static_cast<unsigned char>(chan),0,dest);
    
    start_update_messages(dest);

    //Waiting for MOVE COMPLETE HEADER
    while(!check_code_serport(bsc203_opcodes::MGMSG_MOT_MOVE_HOMED))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //_lg.strm(sl::trace) << "looking for move homed...";
    }

    stop_update_messages(dest);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    _serport->flush();
    
}


void foxtrot::devices::BSC203::set_velocity_params (foxtrot::devices::destination dest, foxtrot::devices::velocity_params* velpar)
{

    auto data = get_velocityparams_data(*velpar);

    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_VELPARAMS, data, dest);


}

void foxtrot::devices::BSC203::set_bowindex(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, int bowindex)
{
    auto data = get_move_request_header_data(bowindex, channel);

    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_BOWINDEX, data, dest);

}

void foxtrot::devices::BSC203::require_status_update(foxtrot::devices::destination dest)
{
    transmit_message(foxtrot::devices::bsc203_opcodes::MGMSG_MOT_REQ_STATUSUPDATE, 0x01, 0, dest);

}

foxtrot::devices::velocity_params foxtrot::devices::BSC203::get_velocity_params(foxtrot::devices::destination dest)
{

    auto out =request_response_struct<velocity_params>(bsc203_opcodes::MGMSG_MOT_REQ_VELPARAMS, bsc203_opcodes::MGMSG_MOT_GET_VELPARAMS, dest, 0x01,0x0);

    //cout << "Velocity params, min vel (hex): " << static_cast<unsigned>(out.minvel) << endl;
    //cout << "Velocity params, max vel (hex): " << static_cast<unsigned>(out.maxvel) << endl;
    
    return out;

}


void foxtrot::devices::BSC203::set_relative_move_params(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, int distance)
{
    auto data = get_move_request_header_data(distance, channel);

    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_MOVERELPARAMS, data, dest);


}

void foxtrot::devices::BSC203::set_absolute_move_params(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, int distance)
{
    auto data = get_move_request_header_data(distance, channel);

    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_MOVEABSPARAMS, data, dest);


}


void foxtrot::devices::BSC203::set_PMD_params(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, const foxtrot::devices::PMDjoystickparams& PMDjoystickstr)
{
    auto data = get_PMDjoystick_params(PMDjoystickstr);

    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_PMDJOYSTICKPARAMS, data, dest);


}

void foxtrot::devices::BSC203::set_jog_params(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, const foxtrot::devices::jogparamsBSC& jogstruct)
{

    auto data = get_jog_set_request_data(jogstruct);

    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_JOGPARAMS, data, dest);

}

foxtrot::devices::jogparamsBSC foxtrot::devices::BSC203::get_jog_params(foxtrot::devices::destination dest)
{
    auto out =request_response_struct<jogparamsBSC>(bsc203_opcodes::MGMSG_MOT_REQ_JOGPARAMS, bsc203_opcodes::MGMSG_MOT_GET_JOGPARAMS, dest, 0x01,0x0);

    //cout << "MoveJogparams, step size (hex): " << std::hex << static_cast<unsigned>(out.jogStepSize) << endl;
    //cout << "MoveJogparams, min vel (hex): " << std::hex << static_cast<unsigned>(out.jogMinVel) << endl;
    
    return out;

}

foxtrot::devices::move_relative_params foxtrot::devices::BSC203::get_relative_move_params(foxtrot::devices::destination dest)
{

    auto out =request_response_struct<move_relative_params>(bsc203_opcodes::MGMSG_MOT_REQ_MOVERELPARAMS, bsc203_opcodes::MGMSG_MOT_GET_MOVERELPARAMS, dest, 0x01,0x0);

    //cout << "MoveRelative params, chan indent (hex): " << std::hex << static_cast<unsigned>(out.chanIndent) << endl;
    //cout << "MoveRelative params, rel distance (hex): " << std::hex << static_cast<unsigned>(out.rel_distance) << endl;
    
    return out;
}


void foxtrot::devices::BSC203::set_poscounter(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, int position)
{

    //Set the channel
    set_channelenable(dest, foxtrot::devices::motor_channel_idents::channel_1,true);

    auto data = get_move_request_header_data(position, channel);

    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_POSCOUNTER, data, dest);

}

unsigned int foxtrot::devices::BSC203::get_poscounter(foxtrot::devices::destination dest)
{

    start_update_messages(dest);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stop_update_messages(dest);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    _serport->flush();

    auto out =request_response_struct<position_counter>(bsc203_opcodes::MGMSG_MOT_REQ_POSCOUNTER, bsc203_opcodes::MGMSG_MOT_GET_POSCOUNTER, dest, 0x01,0x0);

    //cout << "position (hex): " << std::hex << out.position << endl;
    
    return out.position;
}

void foxtrot::devices::BSC203::set_enccounter(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, int enccount)
{
    //Set the channel
    set_channelenable(dest, foxtrot::devices::motor_channel_idents::channel_1,true);

    auto data = get_move_request_header_data(enccount, channel);
    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_ENCCOUNTER, data, dest);

}

unsigned int foxtrot::devices::BSC203::get_enccounter(foxtrot::devices::destination dest)
{

    auto out =request_response_struct<position_counter>(bsc203_opcodes::MGMSG_MOT_REQ_ENCCOUNTER, bsc203_opcodes::MGMSG_MOT_GET_ENCCOUNTER, dest, 0x01,0x0);

    //cout << "position (hex): " << std::hex << out.position << endl;
    
    return out.position;

}

void foxtrot::devices::BSC203::set_homeparams(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel,const foxtrot::devices::homeparams& homestr)
{

    auto data = get_homeparams_data(homestr);
    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_HOMEPARAMS, data, dest);


}

foxtrot::devices::homeparams foxtrot::devices::BSC203::get_homeparams(foxtrot::devices::destination dest)
{
    auto out =request_response_struct<homeparams>(bsc203_opcodes::MGMSG_MOT_REQ_HOMEPARAMS, bsc203_opcodes::MGMSG_MOT_GET_HOMEPARAMS, dest, 0x01,0x0);

    //cout << "home velocity (hex): " << std::hex << out.homeVelocity << endl;
    
    return out;

}

void foxtrot::devices::BSC203::set_power_parameters(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, const foxtrot::devices::powerparams& powerstr)
{

    auto data = get_powerparams_data(powerstr);
    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_POWERPARAMS, data, dest);

}

foxtrot::devices::powerparams foxtrot::devices::BSC203::get_power_parameters(foxtrot::devices::destination dest)
{

    auto out =request_response_struct<powerparams>(bsc203_opcodes::MGMSG_MOT_REQ_POWERPARAMS, bsc203_opcodes::MGMSG_MOT_GET_POWERPARAMS, dest, 0x01,0x0);

    //cout << "chan indent (hex): " << std::hex << out.chanIndent << endl;
    //cout << "rest Factor (hex): " << std::hex << out.restFactor << endl;
    //cout << "move factor (hex): " << std::hex << out.moveFactor << endl;
    
    return out;

}


void foxtrot::devices::BSC203::set_generalmove_params(foxtrot::devices::destination dest, foxtrot::devices::motor_channel_idents channel, int backlashdis)
{

    auto data = get_move_request_header_data(backlashdis, channel);
    transmit_message(bsc203_opcodes::MGMSG_MOT_SET_GENMOVEPARAMS, data, dest);

}

unsigned int foxtrot::devices::BSC203::get_generalmove_params(foxtrot::devices::destination dest)
{

    auto out =request_response_struct<position_counter>(bsc203_opcodes::MGMSG_MOT_REQ_GENMOVEPARAMS, bsc203_opcodes::MGMSG_MOT_GET_GENMOVEPARAMS, dest, 0x01,0x0);

    //cout << "backlash distance (hex): " << std::hex << out.position << endl;
    
    return out.position;
    
}


//Static functions

static std::array<unsigned char, 6> get_powerparams_data ( const foxtrot::devices::powerparams& powerstr)
{
    unsigned char* restFactorbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&powerstr.restFactor));
    unsigned char* moveFactorbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&powerstr.moveFactor));

    std::array<unsigned char, 6> data { static_cast<unsigned char>(powerstr.chanIndent), 0, restFactorbytes[0], restFactorbytes[1], moveFactorbytes[0], moveFactorbytes[1]};

    return data;

}


static std::array<unsigned char, 14> get_homeparams_data(const foxtrot::devices::homeparams& homeparams)
{

    unsigned char* homedirbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&homeparams.homeDir));
    unsigned char* limitswitchbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&homeparams.limitSwitch));
    unsigned char* homevelbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&homeparams.homeVelocity));
    unsigned char* offsetbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&homeparams.offsetDistance));

    std::array<unsigned char, 14> data { static_cast<unsigned char>(homeparams.chanIndent), 0, homedirbytes[0], homedirbytes[1], limitswitchbytes[0], limitswitchbytes[1], homevelbytes[0], homevelbytes[1], homevelbytes[2], homevelbytes[3], offsetbytes[0], offsetbytes[1], offsetbytes[2], offsetbytes[3]};

    return data;

}

static std::array<unsigned char, 14> get_velocityparams_data(const foxtrot::devices::velocity_params& velpar)
{

    unsigned char* minvelbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&velpar.minvel));
    unsigned char* accbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&velpar.acceleration));
    unsigned char* maxvelbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&velpar.maxvel));

    std::array<unsigned char, 14> data { static_cast<unsigned char>(velpar.chan_indent), 0, minvelbytes[0], minvelbytes[1], minvelbytes[2], minvelbytes[3], accbytes[0], accbytes[1], accbytes[2], accbytes[3], maxvelbytes[0], maxvelbytes[1], maxvelbytes[2], maxvelbytes[3]};

    return data;


}

static std::array<unsigned char, 22> get_jog_set_request_data(const foxtrot::devices::jogparamsBSC& jogstructp)
{

    unsigned char* subJogModebytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&jogstructp.jogMode));
    unsigned char* subJogStepSizebytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&jogstructp.jogStepSize));
    unsigned char* subJogMinVelbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&jogstructp.jogMinVel));
    unsigned char* subJogAccnbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&jogstructp.jogAccn));
    unsigned char* subJogMaxVelbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&jogstructp.jogMaxVel));
    unsigned char* subJogStopModebytes = reinterpret_cast<unsigned char *>(const_cast<unsigned short*>(&jogstructp.jogStopMode));


    std::array<unsigned char, 22> data {static_cast<unsigned char>(jogstructp.chanIndent), 0, subJogModebytes[0], subJogModebytes[1], subJogStepSizebytes[0], subJogStepSizebytes[1], subJogStepSizebytes[2], subJogStepSizebytes[3], subJogMinVelbytes[0], subJogMinVelbytes[1], subJogMinVelbytes[2], subJogMinVelbytes[3], subJogAccnbytes[0], subJogAccnbytes[1], subJogAccnbytes[2], subJogAccnbytes[3], subJogMaxVelbytes[0], subJogMaxVelbytes[1], subJogMaxVelbytes[2], subJogMaxVelbytes[3], subJogStopModebytes[0], subJogStopModebytes[1]};

    return data;
}

static std::array<unsigned char, 16> get_limitswitch_params(const foxtrot::devices::limitswitchparams& limitstr)
{
    unsigned char* CWhardbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&limitstr.CWhard));
    unsigned char* CCWhardbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&limitstr.CCWhard));
    unsigned char* CWsoftbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&limitstr.CWsoft));
    unsigned char* CCWsoftbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&limitstr.CCWsoft));
    unsigned char* limitModebytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&limitstr.limitMode));

    std::array<unsigned char, 16> data { static_cast<unsigned char>(limitstr.chan_indent), 0, CWhardbytes[0], CWhardbytes[1], CCWhardbytes[0], CCWhardbytes[1], CWsoftbytes[0], CWsoftbytes[1], CWsoftbytes[2], CWsoftbytes[3], CCWsoftbytes[0], CCWsoftbytes[1], CCWsoftbytes[2], CCWsoftbytes[3], limitModebytes[0], limitModebytes[1]};

    return data;

}

static std::array< unsigned char, 20> get_PMDjoystick_params(const foxtrot::devices::PMDjoystickparams& PMDjoystickstr)
{

    unsigned char* lowMaxVelbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&PMDjoystickstr.lowMaxVel));
    unsigned char* highMaxVelbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&PMDjoystickstr.highMaxVel));
    unsigned char* highLowAccnbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&PMDjoystickstr.highLowAccn));
    unsigned char* highHighAccnbytes = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(&PMDjoystickstr.highHighAccn));
    unsigned char* dirSensebytes = reinterpret_cast<unsigned char*>(const_cast<unsigned short*>(&PMDjoystickstr.dirSense));

    std::array<unsigned char, 20> data { static_cast<unsigned char>(PMDjoystickstr.chan_indent), 0, lowMaxVelbytes[0], lowMaxVelbytes[1], lowMaxVelbytes[2], lowMaxVelbytes[3], highMaxVelbytes[0], highMaxVelbytes[1], highMaxVelbytes[2], highMaxVelbytes[3], highLowAccnbytes[0], highLowAccnbytes[1], highLowAccnbytes[2], highLowAccnbytes[3], highHighAccnbytes[0], highHighAccnbytes[1], highHighAccnbytes[2], highHighAccnbytes[3], dirSensebytes[0], dirSensebytes[1] };

    return data;

}


RTTR_REGISTRATION{
    using namespace rttr;
    using foxtrot::devices::BSC203;
    registration::class_<BSC203>("foxtrot::devices::BSC203")

    .method("identify_module", &BSC203::identify_module)
    (parameter_names("destination (rack)", "channel (channel 1)"))
    .method("set_channelenable", &BSC203::set_channelenable)
    (parameter_names("destination", "channel (channel 1)", "on/off"))
    .method("set_limit_switch_params", &BSC203::set_limit_switch_params)
    (parameter_names("destination", "channel (channel 1)", "limit switch parameters"))
    .method("set_bowindex", &BSC203::set_bowindex)
    (parameter_names("destination", "channel (channel 1)", "bow index"))
    .method("require_status_update", &BSC203::require_status_update)
    (parameter_names("destination"))
    .method("require_digoutputs", &BSC203::require_digoutputs)
    (parameter_names("destination"))
    .method("relative_move", &BSC203::relative_move)
    (parameter_names("destination", "channel (channel 1)", "distance"))
    .method("absolute_move", &BSC203::absolute_move)
    (parameter_names("destination", "channel (channel 1)", "distance"))
    .method("get_bayused_rack", &BSC203::get_bayused_rack)
    (parameter_names("destination", "expected bay"))
    .method("jog_move", &BSC203::jog_move)
    (parameter_names("destination", "channel (channel 1)", "direction"))
    .method("get_status_update", &BSC203::get_status_update)
    (parameter_names("destination", "print (optional)"))
    .method("set_velocity_params", &BSC203::set_velocity_params)
    (parameter_names("destination", "velocity parameters"))
    .method("get_velocity_params", &BSC203::get_velocity_params)
    (parameter_names("destination"))
    .method("set_relative_move_params", &BSC203::set_relative_move_params)
    (parameter_names("destination", "channel (channel 1)", "distance"))
    .method("set_absolute_move_params", &BSC203::set_absolute_move_params)
    (parameter_names("destination", "channel (channel 1)", "distance"))
    .method("set_PMD_params", &BSC203::set_PMD_params)
    (parameter_names("destination", "channel (channel 1)", "PMD joystick parameters"))
    .method("get_relative_move_params", &BSC203::get_relative_move_params)
    (parameter_names("destination"))
    .method("set_jog_params", &BSC203::set_jog_params)
    (parameter_names("destination", "channel (channel 1)", "Jog parameters"))
    .method("get_jog_params", &BSC203::get_jog_params)
    (parameter_names("destination"))
    .method("set_poscounter", &BSC203::set_poscounter)
    (parameter_names("destination", "channel (channel 1)", "position"))
    .method("get_poscounter", &BSC203::get_poscounter)
    (parameter_names("destination"))
    .method("set_enccounter", &BSC203::set_enccounter)
    (parameter_names("destination", "channel (channel 1)", "enccounter"))
    .method("get_enccounter", &BSC203::get_enccounter)
    (parameter_names("destination"))
    .method("set_homeparams", &BSC203::set_homeparams)
    (parameter_names("destination", "channel (channel 1)", "homing parameters"))
    .method("get_homeparams", &BSC203::get_homeparams)
    (parameter_names("destination"))
    .method("set_power_parameters", &BSC203::set_power_parameters)
    (parameter_names("destination", "channel (channel 1)", "power parameters"))
    .method("get_power_parameters", &BSC203::get_power_parameters)
    (parameter_names("destination"))
    .method("set_generalmove_params", &BSC203::set_generalmove_params)
    (parameter_names("destination", "channel (channel 1)", "backslash distance"))
    .method("get_generalmove_params", &BSC203::get_generalmove_params)
    (parameter_names("destination"))
    .method("homing_channel", &BSC203::homing_channel)
    (parameter_names("destination", "channel (channel 1)"));
    
    //Custom structs
    using foxtrot::devices::velocity_params;
    registration::class_<velocity_params>("foxtrot::devices::velocity_params")
    .constructor()(policy::ctor::as_object)
    .property("chan_indent", &velocity_params::chan_indent)
    .property("minvel", &velocity_params::minvel)
    .property("acceleration", &velocity_params::acceleration)
    .property("maxvel", &velocity_params::maxvel);
    
    using foxtrot::devices::move_relative_params;
    registration::class_<move_relative_params>("foxtrot::devices::move_relative_params")
    .constructor()(policy::ctor::as_object)
    .property("chanIndent", &move_relative_params::chanIndent)
    .property("rel_distance", &move_relative_params::rel_distance);
    
    using foxtrot::devices::position_counter;
    registration::class_<position_counter>("foxtrot::devices::position_counter")
    .constructor()(policy::ctor::as_object)
    .property("chanindent", &position_counter::chanindent)
    .property("position", &position_counter::position);
    
    using foxtrot::devices::jogparamsBSC;
    registration::class_<jogparamsBSC>("foxtrot::devices::jogparamsBSC")
    .constructor()(policy::ctor::as_object)
    .property("chanIndent", &jogparamsBSC::chanIndent)
    .property("jogMode", &jogparamsBSC::jogMode)
    .property("jogStepSize", &jogparamsBSC::jogStepSize)
    .property("jogMinVel", &jogparamsBSC::jogMinVel)
    .property("jogAccn", &jogparamsBSC::jogAccn)
    .property("jogMaxVel", &jogparamsBSC::jogMaxVel)
    .property("jogStopMode", &jogparamsBSC::jogStopMode);
    
    using foxtrot::devices::homeparams;
    registration::class_<homeparams>("foxtrot::devices::homeparams")
    .constructor()(policy::ctor::as_object)
    .property("chanIndent", &homeparams::chanIndent)
    .property("homeDir", &homeparams::homeDir)
    .property("limitSwitch", &homeparams::limitSwitch)
    .property("homeVelocity", &homeparams::homeVelocity)
    .property("offsetDistance", &homeparams::offsetDistance);
    
    using foxtrot::devices::powerparams;
    registration::class_<powerparams>("foxtrot::devices::powerparams")
    .constructor()(policy::ctor::as_object)
    .property("chanIndent", &powerparams::chanIndent)
    .property("restFactor", &powerparams::restFactor)
    .property("moveFactor", &powerparams::moveFactor);
    
    using foxtrot::devices::limitswitchparams;
    registration::class_<limitswitchparams>("foxtrot::devices::limitswitchparams")
    .constructor()(policy::ctor::as_object)
    .property("chan_indent", &limitswitchparams::chan_indent)
    .property("CWhard", &limitswitchparams::CWhard)
    .property("CCWhard", &limitswitchparams::CCWhard)
    .property("CWsoft", &limitswitchparams::CWsoft)
    .property("CCWsoft", &limitswitchparams::CCWsoft)
    .property("limitMode", &limitswitchparams::limitMode);
    
    using foxtrot::devices::PMDjoystickparams;
    registration::class_<PMDjoystickparams>("foxtrot::devices::PMDjoystickparams")
    .constructor()(policy::ctor::as_object)
    .property("chan_indent", &PMDjoystickparams::chan_indent)
    .property("lowMaxVel", &PMDjoystickparams::lowMaxVel)
    .property("highMaxVel", &PMDjoystickparams::highMaxVel)
    .property("highLowAccn", &PMDjoystickparams::highLowAccn)
    .property("highHighAccn", &PMDjoystickparams::highHighAccn)
    .property("dirSense", &PMDjoystickparams::dirSense);
    
    //Custom enums
    using foxtrot::devices::channelID;
    registration::enumeration<channelID>("foxtrot::devices::channelID")
    (value("channel_1", channelID::channel_1),
     value("channel_2", channelID::channel_2),
     value("channel_3", channelID::channel_3));

}

