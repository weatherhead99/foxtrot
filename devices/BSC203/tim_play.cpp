#include <iostream>
#include "BSC203.h"
#include "TIM101.h"

#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>

using std::cout;
using std::endl;

foxtrot::parameterset sport_params{
  {"port", "/dev/ttyUSB11"},
  {"flowcontrol", "hardware"}
};


int main(int argc,char** argv)
{
  
  foxtrot::setLogFilterLevel(sl::trace);
  
  cout << "TIM101 test..." << endl;
  
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&sport_params);
  foxtrot::devices::TIM101 motors(sport);
  
  //motors.identify_module(foxtrot::devices::destination::sourceTIM101);
  
  motors.get_hwinfo(foxtrot::devices::destination::sourceTIM101);
  
  //JOG MOVE
  /*foxtrot::devices::jogparams jogstruct;
  jogstruct.subMsgID = 0x0009;
  jogstruct.chanIndent = 0x01;
  jogstruct.jogMode = 0x0002; //step
  jogstruct.jogStepSize = 0x000000AA;
  jogstruct.jogStepRate = 0x000001F4;
  jogstruct.jogStepAccn = 0x000186A0;
  
  motors.set_jog_parameters(foxtrot::devices::destination::sourceTIM101, jogstruct);*/
  
  //motors.jog_move(foxtrot::devices::destination::sourceTIM101,foxtrot::devices::motor_channel_idents::channel_1, foxtrot::devices::jogdir::forward);

  
  //ABSOLUTE MOVE
  /*foxtrot::devices::move_absolute_params absparams;
  absparams.subMsgID = 0x0007;
  absparams.chanIndent = 0x01;
  absparams.maxVoltage = 0x006E;
  absparams.stepRate = 0x00000014;
  absparams.stepAccn = 0x000000F0;
  
  motors.set_move_absolute_parameters(foxtrot::devices::destination::sourceTIM101, absparams);*/
  
  motors.get_status_update();
  
  motors.absolute_move(foxtrot::devices::motor_channel_idents::channel_1, -150);
  
  motors.get_status_update();
  
};
