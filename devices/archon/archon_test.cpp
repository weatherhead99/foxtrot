#include "archon.h"
#include "simpleTCP.h"
#include <memory>
#include <string>
#include <iostream>
#include "archon_modules.h"

#include "TestUtilities.h"


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
  
  
  
  
  
  
  
};
