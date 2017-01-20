#include "archon.h"
#include "archon_modules.h"
#include "archon_module_heaterx.h"

#include <algorithm>

#include "ProtocolUtilities.h"
#include "CommunicationProtocol.h"

#include <locale>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>



#include "ProtocolError.h"

#define READ_SIZE 1024

foxtrot::devices::archon::archon(std::shared_ptr< foxtrot::protocols::simpleTCP > proto)
  : CmdDevice(std::static_pointer_cast<foxtrot::CommunicationProtocol>(proto)), _specproto(proto),
    _order(0)
{
  proto->Init(nullptr);

  //update the stateful dictionaries
  update_state();
  
  //update the modules vector;
  _modules = new std::map<int,std::unique_ptr<ArchonModule>>();
  
  //get the modules_installed string value
  int modules_installed = std::stoi(_system.at("MOD_PRESENT"),0,16);
//   std::cout << "modules_installed: " << modules_installed << std::endl;
  
  
   for(int i = 0; i < 12; i++)
  {
   bool mod_present = ((1 <<i) &  modules_installed) >> i;
   if(mod_present)
   {
//      std::cout << "module present at: " << i+1 << std::endl;
    archon_module_types modtype = static_cast<archon_module_types>(extract_module_variable<short unsigned>(i,"TYPE",getSystem(),'_'));
    
//     std::cout << "modtype: "  << static_cast<short unsigned>(modtype) << std::endl;
    
    std::unique_ptr<ArchonModule> ptr;
    switch(modtype)
    {
        case(archon_module_types::HeaterX):
	  std::cout << "HeaterX module detected at position " << (i+1) << std::endl;	  
            ptr = ArchonHeaterX::constructModule(*this,i);
            _modules->insert(std::pair<int,std::unique_ptr<ArchonModule>>(i, std::move(ptr)));
            break;
	
	default:
	  std::cout << "support for module at position " << (i+1) << " isn't implemented yet"<< std::endl;
	    
    };
    
    
   }
   else
   {
     std::cout << "module not present at position " << (i+1) << std::endl;
   };
   
   
   
  }
  
  
}

devices::archon::~archon()
{
  if(_modules)
  {
    delete _modules;
  };

}



std::string foxtrot::devices::archon::cmd(const std::string& request)
{
  if(_order == 0xFE)
  {
    _order = 0;
  }
  
  auto thisorder = _order;
  
  std::ostringstream oss;
  oss << ">" << std::hex << std::setw(2)<< std::setfill('0') << _order++ << request << "\n";
  
  std::cout << "request is: " << oss.str();

  _specproto->write(oss.str());
  
  std::cout << "command written, waiting for reply..." << std::endl;
  //maximum message size,"<xx:" +  1024 bytes of binary  = 1028
  
  
  
  auto ret = _specproto->read_until_endl();
  
  
  //first characters should be "<xx"
  if(ret[0] != '<')
  {
    throw ProtocolError("invalid archon response!");
  };
  
  auto outret = std::stoul(ret.substr(1,2),nullptr,16);
  std::cout << "outret: " << outret << std::endl;
  
  if(outret != thisorder)
  {
    throw ProtocolError("mismatched order response");
  }
  
  //-3: 2 for the initial 2 chopped off, 1 for chopping off the endl at the end
  return ret.substr(3);

}

ssmap devices::archon::parse_parameter_response(const std::string& response)
{
  
  ssmap out;
  std::istringstream ss(response);
  std::string item;
  
  while(std::getline(ss, item, ' '))
  {
    auto eqpos = std::find(item.begin(), item.end(),'=');
    if(eqpos == item.end())
    {
      std::cout << "problematic item: _" << item << "_" << std::endl;
      std::cout << "ignoring" << std::endl;
      
      continue;
//       throw ProtocolError("malformed archon parameter response");
    };
    
    std::string key(item.begin(), eqpos);
    
    //tidy up strings
//     std::cout << "key before:" << key << "\t length:" << key.size() << std::endl;
    key.erase(std::remove_if(key.begin(),key.end(),[](char c) {return !std::isprint(c); }),key.end());
    
//     std::cout << "keey after: " << key <<  "\t length:" << key.size() << std::endl;
    std::string val(eqpos + 1, item.end());
    val.erase(std::remove_if(key.begin(),key.end(),[](char c) {return !std::isprint(c); }),key.end());
    
    out[key] = val;
    
  };
  return out;

}

const ssmap& devices::archon::getStatus() const
{
  return _status;

}

const ssmap& devices::archon::getSystem() const
{
  return _system;
}


void devices::archon::update_state()
{
  _system = parse_parameter_response(cmd("SYSTEM"));
  _status = parse_parameter_response(cmd("STATUS"));

}

// WARNING: VERY BAD CODE HERE!
const std::map<int, const foxtrot::devices::ArchonModule&> foxtrot::devices::archon::getAllModules() const
{
    std::map<int, const foxtrot::devices::ArchonModule&> out;
    
    for(const auto& mod : *_modules)
    {
        out.insert( std::pair<int,foxtrot::devices::ArchonModule&>(mod.first, *mod.second.get())  );
    }
    
    return out;
}

