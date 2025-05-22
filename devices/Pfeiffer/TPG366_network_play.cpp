#include <iostream>
#include "TPG362.h"

#include <foxtrot/protocols/simpleTCP.h>
#include <foxtrot/Logging.h>

#include <memory>

#include <foxtrot/backward.hpp>

using std::cout;
using std::endl;


foxtrot::parameterset tcp_params {
  {"port", 8000},
  {"addr", "131.215.200.34"}
};

int main(int argc, char** argv)
{
  backward::SignalHandling sh;

  foxtrot::setLogFilterLevel(sl::trace);
  cout << "initializing TCP stream..." << endl;
  auto port = std::make_shared<foxtrot::protocols::simpleTCP>(&tcp_params);

  cout << "initializing vacuum gauge controller... "<< endl;
  foxtrot::devices::TPG362 vacuumgauge(port);

  cout << "vacuum gauge ready" << endl;

  auto HWVersion = vacuumgauge.HWVersion();

  cout << "vacuum gauge hardware version: " << HWVersion << endl;
  cout << "serial number: " << vacuumgauge.SerialNumber() << endl;
  cout << "Device Name: " << vacuumgauge.getDeviceName(0) << endl;
  cout << "Sensor name: " << vacuumgauge.getDeviceName(1) << endl;
  cout << "invalid!" << vacuumgauge.getDeviceName(7) << endl;
}
