#include <iostream>
#include "TPG362.h"
#include "SerialPort.h"
#include <memory>

using std::cout;
using std::endl;

foxtrot::parameterset sport_params {
  {"port" , "/dev/ttyUSB0"},
  {"baudrate" , 115200u},
  };
  


int main(int argc, char** argv)
{
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&sport_params);
  
  
  foxtrot::devices::TPG362 vacuumgauge(sport);
  
}