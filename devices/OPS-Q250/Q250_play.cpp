#include <iostream>

#include "scsiserial.h"
#include "Q250.h"

#include <chrono>
#include <thread>
#include <memory>

using std::cout;
using std::endl;

void print_lamp_op(foxtrot::devices::Q250& psu)
{
 cout << "amps: " << psu.getAmps() << " volts: " << psu.getVolts() << " watts:  " << psu.getWatts() << endl; 
}

int main(int argc, char** argv)
{
  
  foxtrot::parameterset params{ 
    {"devnode", "/dev/sdb"},
    {"timeout", 2000u},
  };
  

  auto proto = std::make_shared<foxtrot::protocols::scsiserial>(&params);
  foxtrot::devices::Q250 psu(proto);
  
  
  
  
  proto->write("IDN?");
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  auto idn = proto->read(512);
  cout << "idn: " << idn << endl;
  
  print_lamp_op(psu);
  
  cout << "starting lamp..." << endl;
  psu.start();
  
  for(int i=0 ;i < 20; i++)
  {
    cout << "i: " << i << endl;
    print_lamp_op(psu);
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
  
  cout << "stopping lamp..." << endl;
  psu.stop();
  
  for(int i=0 ;i < 20; i++)
  {
    cout << "i: " << i << endl;
    print_lamp_op(psu);
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
  
}