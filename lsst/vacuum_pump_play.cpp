#include <iostream>
#include <foxtrot/devices/TC110.h>
#include <foxtrot/protocols/SerialPort.h>
#include <memory>

using std::cout;
using std::endl;


foxtrot::parameterset sport_params {
  {"port", "/dev/ttyUSB1"},
    {"baudrate", 9600u}};


int main(int argc, char** argv)
{
  cout << "hello" << endl;

  cout << "initializing serial port... "<< endl;
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&sport_params);

  cout << "initializing pump interface...." << endl;
  auto pump = std::unique_ptr<foxtrot::devices::TC110>(new foxtrot::devices::TC110(sport));
  
  cout << "actual speed: " << pump->getActualRotSpeed() << endl;
  pump->setVentEnable(false);

}
