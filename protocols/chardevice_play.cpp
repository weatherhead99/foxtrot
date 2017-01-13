#include "characterdevice.h"
#include <iostream>



using std::cout; 
using std::endl;

int main(int argc, char** argv)
{
  foxtrot::parameterset params;
  params["devnode"] = "/dev/usbtmc0";
  
  
  cout << "char device init..." <<endl;
  
  foxtrot::protocols::characterdevice dev(&params);
  dev.Init(nullptr);
  
  cout << "char device write..." << endl;
  dev.write(":MEAS:FRES?");
  
  cout << "char device read..." << endl;
  auto repl = dev.read_until_endl();
  
  
  cout << "reply: " << repl << endl;
}