#include "ssc_tcube_edge.hh"
#include <iostream>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>

using std::cout;
using std::endl;

using foxtrot::protocols::SerialPort;

foxtrot::parameterset sport_params
  {
    { "port", "/dev/ttyUSB1"},
    { "baudrate", 57600u}
  };


int main()
{
  foxtrot::setLogFilterLevel(sl::trace);
  cout << "hello" << endl;

  auto sport = std::make_shared<SerialPort>(&sport_params);
  foxtrot::devices::TCubeEdge chiller(sport);


  cout << chiller.identify() << endl;
  cout << "set temp: " << chiller.setpoint_temp() << endl;
  cout << "measured temp:" << chiller.RTD_temp() << endl;
  cout << "PWM% : " << chiller.TE_PWM() << endl;


  cout << "setting temp...";
  chiller.set_setpoint_temp(0.0);
  
  

}
  
