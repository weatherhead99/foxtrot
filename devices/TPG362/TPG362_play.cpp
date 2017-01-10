#include <iostream>
#include "TPG362.h"
#include "SerialPort.h"
#include <memory>

using std::cout;
using std::endl;

foxtrot::parameterset sport_params {
  {"port" , "/dev/ttyUSB0"},
  {"baudrate" , 9600u},
  };
  


int main(int argc, char** argv)
{
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&sport_params);
  
  
  foxtrot::devices::TPG362 vacuumgauge(sport);

  
  std::cout << "gauge 1 (" << vacuumgauge.getDeviceName(1) <<") active: " << vacuumgauge.getGaugeOnOff(1) << " pressure: "  << vacuumgauge.getPressure(1) << " hPa"  << std::endl;
  std::cout << "gauge 2 (" << vacuumgauge.getDeviceName(2) <<") active: " << vacuumgauge.getGaugeOnOff(2) << " pressure: "  << vacuumgauge.getPressure(2) << " hPa"  << std::endl;
  
}
