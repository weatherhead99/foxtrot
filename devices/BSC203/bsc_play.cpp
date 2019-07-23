#include <iostream>
#include "BSC203.h"
#include "TIM101.h"

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <unistd.h>

using std::cout;
using std::endl;

foxtrot::parameterset sport_params {
    {"port", "/dev/ttyUSB1"},
    {"flowcontrol", "hardware"}
};


int main(int argc,char** argv)
{

    foxtrot::setLogFilterLevel(sl::trace);

    cout << "BSC203 test..." << endl;


    auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&sport_params);
    foxtrot::devices::BSC203 motors(sport);

    //GENERAL THINGS
    //motors.identify_module(foxtrot::devices::destination::rack, foxtrot::devices::channelID::channel_1);

    //motors.get_hwinfo(foxtrot::devices::destination::rack);

    //motors.set_channelenable(foxtrot::devices::destination::bay1, foxtrot::devices::motor_channel_idents::channel_1, true);

    //RELATIVE & ABSOLUTE MOVE
    motors.set_channelenable(foxtrot::devices::destination::bay2, foxtrot::devices::motor_channel_idents::channel_1, true);
    motors.set_channelenable(foxtrot::devices::destination::bay3, foxtrot::devices::motor_channel_idents::channel_1, true);
    
    //motors.get_enccounter(foxtrot::devices::destination::bay3);

    //motors.get_status_update(foxtrot::devices::destination::bay2);

    for (int i = 0; i < 10; i++){
        motors.relative_move(foxtrot::devices::destination::bay2, foxtrot::devices::motor_channel_idents::channel_1, 409600 * 2);
        motors.relative_move(foxtrot::devices::destination::bay3, foxtrot::devices::motor_channel_idents::channel_1, 409600 * 2);
    }
    
    //motors.get_status_update(foxtrot::devices::destination::bay2);
    //motors.get_status_update(foxtrot::devices::destination::bay3);

    //motors.absolute_move(foxtrot::devices::destination::bay2, foxtrot::devices::motor_channel_idents::channel_1, 409600*(3));

    //JOG MOVE
    /*motors.get_status_update(foxtrot::devices::destination::bay3);

    motors.jog_move(foxtrot::devices::destination::bay3, foxtrot::devices::motor_channel_idents::channel_1, 0x01);*/

    //SET POSITION
    /*motors.set_poscounter(foxtrot::devices::destination::bay3, foxtrot::devices::motor_channel_idents::channel_1, 0x0);

    motors.get_status_update(foxtrot::devices::destination::bay3);*/

    //SET ENCCOUNTER
    /*motors.set_enccounter(foxtrot::devices::destination::bay3, foxtrot::devices::motor_channel_idents::channel_1, 0x05);

    //motors.jog_move(foxtrot::devices::destination::bay3, foxtrot::devices::motor_channel_idents::channel_1, 0x01);

    motors.get_enccounter(foxtrot::devices::destination::bay3);*/

    //motors.get_status_update(foxtrot::devices::destination::bay3);

    //HOMING
    //motors.set_channelenable(foxtrot::devices::destination::bay2, foxtrot::devices::motor_channel_idents::channel_1, true);

    //motors.homing_channel(foxtrot::devices::destination::bay2, foxtrot::devices::motor_channel_idents::channel_1);


};
