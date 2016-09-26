#include <iostream>
#include <memory>
#include "BulkUSB.h"

#include "newport2936R.h"


int main(int argc, char** argv)
{
  
     auto proto = std::make_shared<foxtrot::protocols::BulkUSB>(nullptr);
     foxtrot::devices::newport2936R OPM(proto);
     auto repl = OPM.cmd("PM:P?");
     std::cout << repl << std::endl;
     std::cout << "reply length: " << repl.size() << std::endl;
  
  
     auto lambda = OPM.getLambda();
     std::cout << lambda << std::endl;
  
     OPM.setLambda(770);
     lambda = OPM.getLambda();
     std::cout << lambda << std::endl;
     
     
     auto power = OPM.getPower();
     std::cout << power << std::endl;
     
     auto resp = OPM.getResponsivity();
     std::cout << resp << std::endl;
     
     auto area = OPM.getArea();
     std::cout << area << std::endl;
     
}