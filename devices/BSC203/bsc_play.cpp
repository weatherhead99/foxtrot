#include <iostream>
#include "BSC203.h"
#include "TIM101.h"

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>

using std::cout;
using std::endl;

foxtrot::parameterset sport_params{
  {"port", "/dev/ttyUSB0"},
  {"flowcontrol", "hardware"}
};


int main(int argc,char** argv)
{
    
  foxtrot::setLogFilterLevel(sl::trace);
    
  cout << "BSC203/TIM101 test..." << endl;
  
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&sport_params);
  foxtrot::devices::TIM101 motors(sport);
  
  //motors.identify_module(foxtrot::devices::destination::sourceTIM101);
  
  //auto hwinfo = motors.get_hwinfo(foxtrot::devices::destination::sourceTIM101);
  //printhwinfo(hwinfo);
  
  //JOG MOVE
  foxtrot::devices::jogparams jogstruct;
  jogstruct.subMsgID = 0x0009;
  jogstruct.chanIndent = 0x01;
  jogstruct.jogMode = 0x0002; //continuous
  jogstruct.jogStepSize = 0x000000AA;
  jogstruct.jogStepRate = 0x000001F4;
  jogstruct.jogStepAccn = 0x000186A0;
  
  motors.set_jog_parameters(foxtrot::devices::destination::sourceTIM101,&jogstruct);
  
  foxtrot::devices::jogparams jogstruct_req = motors.request_jog_parameters(foxtrot::devices::destination::sourceTIM101);
  
  cout << "jog Mode requested: " << std::hex << jogstruct_req.jogMode << endl;
  cout << "jog Mode Size requested: " << std::hex << jogstruct_req.jogStepSize << endl;
  cout << "jog Step Accn requested: " << std::hex << jogstruct_req.jogStepAccn << endl;
  
  motors.jog_move(foxtrot::devices::destination::sourceTIM101,foxtrot::devices::motor_channel_idents::channel_1, foxtrot::devices::jogdir::forward);

  //ABSOLUTE MOVE
  //motors.absolute_move(foxtrot::devices::destination::sourceTIM101,foxtrot::devices::motor_channel_idents::channel_1, 100);
  

};
