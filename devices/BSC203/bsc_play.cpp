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
  /*foxtrot::devices::jogparams jogstruct;
  jogstruct.subMsgID = 0x0009;
  jogstruct.chanIndent = 0x01;
  jogstruct.jogMode = 0x0002; //step
  jogstruct.jogStepSize = 0x0000000A;
  jogstruct.jogStepRate = 0x000001F4;
  jogstruct.jogStepAccn = 0x000186A0;
  
  motors.set_jog_parameters(foxtrot::devices::destination::sourceTIM101,jogstruct);
  
  foxtrot::devices::jogparams jogstruct_req = motors.request_jog_parameters(foxtrot::devices::destination::sourceTIM101);
  
  cout << "jog Mode requested: " << std::hex << jogstruct_req.jogMode << endl;
  cout << "jog Step Size requested: " << std::hex << jogstruct_req.jogStepSize << endl;
  cout << "jog Step Accn requested: " << std::hex << jogstruct_req.jogStepAccn << endl;*/
  
  //motors.jog_move(foxtrot::devices::destination::sourceTIM101,foxtrot::devices::motor_channel_idents::channel_1, foxtrot::devices::jogdir::forward);

  // SET POSITION COUNTER
  foxtrot::devices::pos_counter_params posparams;
  posparams.subMsgID = 0x0005;
  posparams.chanIndent = 0x08;
  posparams.position = 0x0;
  posparams.encCount = 0x0;
  
  motors.set_position_counter(foxtrot::devices::destination::sourceTIM101, posparams);
  
  motors.jog_move(foxtrot::devices::destination::sourceTIM101,foxtrot::devices::motor_channel_idents::channel_1, foxtrot::devices::jogdir::forward);

  motors.jog_move(foxtrot::devices::destination::sourceTIM101,foxtrot::devices::motor_channel_idents::channel_4, foxtrot::devices::jogdir::forward);
  
  //foxtrot::devices::pos_counter_params posparams_rec = motors.request_position_counter(foxtrot::devices::destination::sourceTIM101);
  //cout << "Get position counter says: " << std:: hex << posparams_rec.position << endl;
  
  
  //ABSOLUTE MOVE
  /*foxtrot::devices::move_absolute_params absparams;
  absparams.subMsgID = 0x0007;
  absparams.chanIndent = 0x01;
  absparams.maxVoltage = 0x006E;
  absparams.stepRate = 0x00000014;
  absparams.stepAccn = 0x0000000F;
  
  motors.set_move_absolute_parameters(foxtrot::devices::destination::sourceTIM101, absparams);
  
  foxtrot::devices::move_absolute_params absparams_rec = motors.request_move_absolute_parameters(foxtrot::devices::destination::sourceTIM101);
  
  cout << "move_absolute channel : " << std::hex << absparams_rec.chanIndent << endl;
  cout << "move_absolute max voltage : " << std::hex << absparams_rec.maxVoltage << endl;
  cout << "move_absolute step rate : " << std::hex << absparams_rec.stepRate << endl;
  cout << "move_absolute step accn : " << std::hex << absparams_rec.stepAccn << endl;*/
  
  //motors.get_status_update(foxtrot::devices::destination::sourceTIM101);
  
  //foxtrot::devices::pos_counter_params posparams_rec = motors.request_position_counter(foxtrot::devices::destination::sourceTIM101);
  //cout << "Get position counter says: " << std:: hex << posparams_rec.position << endl;

  
  //motors.absolute_move(foxtrot::devices::destination::sourceTIM101,foxtrot::devices::motor_channel_idents::channel_1, 0x64);
  
  //foxtrot::devices::motor_status motorstr = motors.get_status_update(foxtrot::devices::destination::sourceTIM101);

};
