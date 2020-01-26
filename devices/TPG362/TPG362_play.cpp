#include <iostream>
#include "TPG362.h"
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <memory>
#include <foxtrot/backward.hpp>

using std::cout;
using std::endl;

foxtrot::parameterset sport_params {
  {"port" , "/dev/ttyUSB11"},
  {"baudrate" , 9600u},
  };
  


int main(int argc, char** argv)
{
  backward::SignalHandling sh;

  cout << "setting log filter level" << endl;
  foxtrot::setLogFilterLevel(sl::trace);
  
  cout << "initializing serial port" << endl;
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&sport_params);
  
  
  cout << "init vacuum gauge" << endl;
  foxtrot::devices::TPG362 vacuumgauge(sport);

  
  cout << "vacuum gauge ready" << endl;
  

  //   std::cout << "name channel 0: " << vacuumgauge.getDeviceName(0) << endl;
  cout << "name channel 2 : " <<   vacuumgauge.getDeviceName(2) << endl;
  
//  std::cout << "gauge 1 (" << vacuumgauge.getDeviceName(1) <<") active: " << vacuumgauge.getGaugeOnOff(1) << " pressure: "  << vacuumgauge.getPressure(1) << " hPa"  << std::endl;
  std::cout << "gauge 2 (" << vacuumgauge.getDeviceName(2) <<") active: " << vacuumgauge.getGaugeOnOff(2) << " pressure: "  << vacuumgauge.getPressure(2) << " hPa"  << std::endl;
//   
}
