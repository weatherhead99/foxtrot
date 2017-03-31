#include <iostream>
#include <memory>
#include "BulkUSB.h"
#include "SerialPort.h"
#include <chrono>
#include <thread>

#include "newport2936R.h"
#include <backward.hpp>
#include "Logging.h"

int main(int argc, char** argv)
{
     backward::SignalHandling sh;
     foxtrot::setDefaultSink();
     
     foxtrot::parameterset params
     { {"port", "/dev/ttyS5"},
       {"read_timeout", 1000u}
       
     };
  
//      auto proto = std::make_shared<foxtrot::protocols::BulkUSB>(&params);
     auto proto = std::make_shared<foxtrot::protocols::BulkUSB>(&params);
     foxtrot::devices::newport2936R OPM(proto);
     
     bool ok;
     auto ampstr = convert_powerunit_to_string(foxtrot::devices::powerunits::Amps, ok);
     std::cout << "amps: " << ampstr << std::endl;
     
     std::cout << "manual command..>" << std::endl;
     auto repl = OPM.cmd("PM:P?");
     std::cout << repl << std::endl;
     std::cout << "reply length: " << repl.size() << std::endl;
  
      std::cout << "auto commands..." << std::endl;
     
     auto lambda = OPM.getLambda();
     std::cout << lambda << std::endl;
  
     OPM.setLambda(770);
     lambda = OPM.getLambda();
     std::cout << lambda << std::endl;
     
     for(int i=0; i< 50.; i++)
     {
     
	auto power = OPM.getPower();
	std::cout << power << std::endl;
// 	auto resp = OPM.getResponsivity();
// 	std::cout << resp << std::endl;
//      
// 	auto area = OPM.getArea();
// 	std::cout << area << std::endl;
//      
	
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::cout << "finished." << std::endl;
    
}