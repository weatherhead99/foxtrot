#include "archon.h"
#include "simpleTCP.h"
#include <memory>
#include <string>
#include <iostream>



class archon_harness : public foxtrot::devices::archon
{
public:
  archon_harness(std::shared_ptr< simpleTCP > proto)
  : archon(proto) 
  {};
  std::string archoncmd(const std::string& request)
  {
   return archon::archoncmd(request); 
  }
};


int main(int argc, char** argv)
{
  foxtrot::parameterset params;
  params["addr"] = "10.0.0.2";
  params["port"] = 4242u;
  
  std::shared_ptr<foxtrot::protocols::simpleTCP> proto(new foxtrot::protocols::simpleTCP(&params));
  
  std::cout << "init archon... " << std::endl;
  
  archon_harness a(proto);
  
  std::cout << "sending STATUS command" << std::endl;

  auto repl = a.archoncmd("STATUS");
  
  std::cout << "response to STATUS:" << repl << std::endl;
};
