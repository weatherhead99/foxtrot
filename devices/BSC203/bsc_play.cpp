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
  
  auto hwinfo = motors.get_hwinfo(foxtrot::devices::destination::sourceTIM101);
  printhwinfo(hwinfo);
  
  motors.jog_move(foxtrot::devices::destination::sourceTIM101,foxtrot::devices::motor_channel_idents::channel_4, foxtrot::devices::jogdir::reverse);

  
  //motors.absolute_move(foxtrot::devices::destination::sourceTIM101,foxtrot::devices::motor_channel_idents::channel_1, 100);
  
  /*motors.set_channelenable(foxtrot::devices::destination::sourceTIM101, foxtrot::devices::motor_channel_idents::channel_1, false);
  
  bool chanEn = motors.get_channelenable(foxtrot::devices::destination::sourceTIM101, foxtrot::devices::motor_channel_idents::channel_1);
  
  if (chanEn){
      cout << "enabled\n";
  }else{
      cout <<"not enabled\n";
  }*/

};
