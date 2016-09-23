#include <iostream>
#include <memory>
#include "BulkUSB.h"

#include "newport2936R.h"


int main(int argc, char** argv)
{
  
     auto proto = std::make_shared<foxtrot::protocols::BulkUSB>(nullptr);
     foxtrot::devices::newport2936R OPM(proto);
     auto repl = OPM.cmd("PM:P?");
     
     for(auto& c: repl)
     {
       std::cout << (unsigned) c << " ";
     }
     std::cout << std::endl;
     
  
  
}