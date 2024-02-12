#include "RLY_8.hh"
#include <iostream>
#include <foxtrot/protocols/SerialPort.h>
#include <foxtrot/Logging.h>
#include <foxtrot/backward.hpp>
#include <chrono>
#include <thread>
#include <bitset>

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

  auto state = dev.getRelayState();
  cout << "relay state: " << std::hex << (int) state << endl;

  dev.setRelay(1, true);


  cout << "wrote relay state" << endl;
  state = dev.getRelayState();
  
  cout << "relay state:" << std::hex << (int) state << endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  dev.setRelay(1, false);


  for(int i=0; i < 10; i++)
    {
      
      for(int r = 1; r <=8; r++)
	{
	  auto rst = dev.getRelayState();
	  auto bs = std::bitset<8>(rst);
	  cout << "r: " << i << "value we think: " << bs[r-1];

	  dev.setRelay(r, !bs[r-1]);
	  std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
      
    }


  cout << "version: " << dev.getVersion() << endl;
  
}
