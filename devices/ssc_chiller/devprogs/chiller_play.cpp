#include "ssc_uc160.hh"
#include <iostream>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
using std::cout;
using std::endl;


foxtrot::parameterset sport_params
  {
    {"port", "/dev/ttyUSB0"}

  };

using foxtrot::protocols::SerialPort;

int main()
{

  foxtrot::setLogFilterLevel(sl::trace);
  cout << "hello" << endl;


  auto sport = std::make_shared<SerialPort>(&sport_params);

  foxtrot::devices::UC170Chiller chiller(sport);
  
  auto temp = chiller.get_ActualTemp();

  cout <<"actual temperature:" << temp << endl;
  

}
