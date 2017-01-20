#include "archon.h"
#include "simpleTCP.h"
#include <memory>
#include <string>
#include <iostream>
#include "archon_modules.h"
#include "archon_module_heaterx.h"
#include "TestUtilities.h"
#include "backward.hpp"

#define BACKWARD_HAS_BFD 1

// class archon_harness : public foxtrot::devices::archon
// {
// public:
//   archon_harness(std::shared_ptr< simpleTCP > proto)
//   : archon(proto) 
//   {};
//   std::string archoncmd(const std::string& request)
//   {
//    return archon::cmd(request); 
//   }
//   
//   ssmap parse_parameter_response(const std::string& response)
//   {
//    return archon::parse_parameter_response(response); 
//   }
//   
// };


int main(int argc, char** argv)
{
  backward::SignalHandling sh;
  foxtrot::parameterset params;
  params["addr"] = "10.0.0.2";
  params["port"] = 4242u;
  
  std::shared_ptr<foxtrot::protocols::simpleTCP> proto(new foxtrot::protocols::simpleTCP(&params));
  
  std::cout << "init archon... " << std::endl;
  
  foxtrot::devices::archon a(proto);
  
  
  auto system = a.getSystem();
  for (auto& item: system)
  {
   std::cout << item.first << "\t" << item.second << std::endl; 
    
  }
  
  
  auto modules = a.getAllModules();
  for( auto& mod: modules)
  {
    std::cout << "module in position: " << mod.first << std::endl;
    
  };
  
  
  auto heater = static_cast<const foxtrot::devices::ArchonHeaterX*>(&modules.at(10));
  
  std::cout << "tempA:" << heater->getTempA() << std::endl;
  std::cout << "tempB:" << heater->getTempB() << std::endl;
  std::cout << "tempC:" << heater->getTempC() << std::endl;
  
  
};
