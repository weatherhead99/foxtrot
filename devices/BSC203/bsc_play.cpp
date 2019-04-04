#include <iostream>
#include "BSC203.h"
#include "SerialPort.h"

using std::cout;
using std::endl;

foxtrot::parameterset sport_params{
  {"port", "/dev/ttyUSB10"},
  {"flowcontrol", "hardware"}
};


int main(int argc,char** argv)
{
  cout << "BSC203 test..." << endl;
  
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&sport_params);
  foxtrot::devices::BSC203 motors(sport);
  
  auto hwinfo = motors.get_hwinfo(foxtrot::devices::destination::rack);
  cout << "serial#: " << hwinfo.serno << endl;
  

};
