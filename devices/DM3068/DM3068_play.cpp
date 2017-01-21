#include "DM3068.h"
#include <iostream>
#include "characterdevice.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  
  foxtrot::parameterset params;
  params["devnode"] = "/dev/usbtmc0";
  
  
  cout << "char device init..." <<endl;
  
  auto proto =std::make_shared<foxtrot::protocols::characterdevice>(&params);
  
  foxtrot::devices::DM3068 multimeter(proto);
  
   auto idn = multimeter.cmd("*IDN?");
   cout << idn << endl;

   auto repl = multimeter.cmd(":MEAS:FRES?");
   cout << repl << endl;
   
  cout << multimeter.get4WireResistance() << endl;
}
  