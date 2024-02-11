#include "RLY_8.hh"
#include <iostream>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <foxtrot/backward.hpp>
#include <chrono>
#include <thread>

using std::cout;
using std::endl;

foxtrot::parameterset sport_params
  {
    {"port", "/dev/ttyACM0"},
    {"baudrate" , 115200}
  };

int main()
{
  backward::SignalHandling sh;
  foxtrot::setLogFilterLevel(sl::trace);
  using foxtrot::protocols::SerialPort;
  auto proto = std::make_shared<SerialPort>(&sport_params);


  foxtrot::devices::RLY_8 dev(proto);

  cout << "getting version..." << endl;
  auto vers = dev.getVersion();

  cout << "version: " << vers << endl;

  cout << "relay state: " << std::hex << (int) dev.getRelayState() << endl;

  dev.setRelay(1, true);

  cout << "relay state:" << std::hex << (int) dev.getRelayState() << endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  dev.setRelay(1, false);
  
}
