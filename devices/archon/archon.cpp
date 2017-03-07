#include "archon.h"
#include "archon_modules.h"
#include "archon_module_heaterx.h"
#include "DeviceError.h"

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

const string devices::archon::getDeviceTypeName() const
{
  return "archon";
}


foxtrot::devices::archon::archon(std::shared_ptr< foxtrot::protocols::simpleTCP > proto)
  : CmdDevice(std::static_pointer_cast<foxtrot::CommunicationProtocol>(proto)), _specproto(proto),
    _order(0)
{
  proto->Init(nullptr);

  //update the stateful dictionaries
  update_state();
  
  //update the modules vector;
//   _modules = new std::map<int,std::unique_ptr<ArchonModule>>();
  
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
            _modules.insert(std::pair<int,std::unique_ptr<ArchonModule>>(i, std::move(ptr)));
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
  
  //TODO: clear config on init, because otherwise we have no idea what's in there
  clear_config();
  
  //setup lines, timing lines etc...
  set_timing_lines(0);
  set_states(0);
  set_parameters(0); 
  set_constants(0);
  
  
}

devices::archon::~archon()
{
  
}



std::string foxtrot::devices::archon::cmd(const std::string& request)
{
  if(_order == 0xFE)
  {
    _order = 0;
  }
  
  auto thisorder = _order;
  
  std::ostringstream oss;
  oss << ">" <<std::uppercase << std::hex << std::setw(2)<< std::setfill('0') << _order++ << request << "\n";
  
//   std::cout << "request is: " << oss.str();

  _specproto->write(oss.str());
  
//   std::cout << "command written, waiting for reply..." << std::endl;
  //maximum message size,"<xx:" +  1024 bytes of binary  = 1028
  
  
  
  auto ret = _specproto->read_until_endl();
  
  //sanitize response
  ret.erase(std::remove_if(ret.begin(),ret.end(),[] (char c) { return !std::isprint(c); }),ret.end());
  
  
  
  if(ret[0] != '<' )
  {
   
    std::cout << "got RET: " << ret << std::endl;
    std::cout << "request was: " << request << std::endl;
    if(ret[0] == '?')
    {
      
     throw DeviceError("archon threw an error message! Check the logs..."); 
    }
    
    throw ProtocolError("invalid archon response!");
  };
  
  
  
  auto outret = std::stoul(ret.substr(1,2),nullptr,16);
//   std::cout << "outret: " << outret << std::endl;
  
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
  _frame = parse_parameter_response(cmd("FRAME"));
  
  for(auto& mod: _modules)
  {
    if(mod.second != nullptr)
    {
     mod.second->update_variables(); 
    }
  }
  
}

// WARNING: VERY BAD CODE HERE!
const std::map<int, foxtrot::devices::ArchonModule&> foxtrot::devices::archon::getAllModules() const
{
    std::map<int, foxtrot::devices::ArchonModule&> out;
    
    for(const auto& mod : _modules)
    {
        out.insert( std::pair<int,foxtrot::devices::ArchonModule&>(mod.first, *mod.second.get())  );
    }
    
    return out;
}

void devices::archon::clear_config()
{
  cmd("CLEARCONFIG");
  _config_lines = 0;

}


int devices::archon::writeConfigLine(const string& line,int num)
{
  //check if possible
  if(_config_lines >= 2047)
  {
   throw foxtrot::DeviceError("tried to write too many config lines to archon");
  }
  if(line.size() >= 2047)
  {
   throw foxtrot::DeviceError("tried to write a config line to archon that was too long"); 
  }
  
  if(num <0)
  {
    //if the value -1 is passed in, add a new line
    num = _config_lines++;
    
  }
  else if(num > _config_lines)
  {
    throw std::logic_error("trying to overwrite a config line" + std::to_string(num) + " that doesn't exist yet");
  };
  
  std::ostringstream oss;
  oss << "WCONFIG" << std::setw(4) << std::setfill('0') << std::uppercase<<  std::hex << num << line;
  cmd(oss.str());
  
  return num;

}



std::string devices::archon::readConfigLine(int num)
{
  if( num > _config_lines)
  {
    throw std::logic_error("tried to read an archon config line that we don't think exists...");
  };
  
  std::ostringstream oss;
  oss << "RCONFIG" << std::setw(4) << std::setfill('0') << std::hex << num ;
  auto repl = cmd(oss.str());
  
  return repl;

}


void devices::archon::applyall()
{
  cmd("APPLYALL");

}




std::string devices::archon::fetch_log()
{
  auto repl = cmd("FETCHLOG");
  return repl;

}

std::vector< string > devices::archon::fetch_all_logs()
{
  update_state();
  auto statmap = getStatus();
  int nLogs = std::stoi(statmap.at("LOG"));
  
  std::vector<std::string> out;
  out.reserve(nLogs);
  for(int i=0; i < nLogs; i++)
  {
    out.push_back(fetch_log());
  };

  return out;
}



string devices::archon::readKeyValue(const string& key)
{
  auto linenumit = _configlinemap.find(key);
  if(linenumit == _configlinemap.end())
  {
    throw std::runtime_error("the key " + key + " is not registered with this archon handler");
  };
  
  auto line = readConfigLine(linenumit->second);
  
  //fine equals sign
  auto eqpos = std::find(line.begin(), line.end(),'=');
  if(eqpos == line.end())
  {
    throw DeviceError("malformed config line returned from archon");
    
  };
  
  auto val = string(eqpos+1,line.end());
  
  return val;
  
}

void devices::archon::writeKeyValue(const string& key, const string& val)
{
  auto linenumit = _configlinemap.find(key);
  
  std::ostringstream oss; 
  oss << key << "=" << val;
  
  if(linenumit == _configlinemap.end())
  {
    //this is a new key 
    auto linenum = writeConfigLine(oss.str());
    _configlinemap.insert({key,linenum});
  
  }
  else
  {
    auto linenum = _configlinemap.at(key);
    writeConfigLine(oss.str(),linenum);
  };
}


void devices::archon::set_timing_lines(int n)
{
  writeKeyValue("LINES",std::to_string(n));

}

int devices::archon::get_timing_lines()
{
  return std::stoi(readKeyValue("LINES"));

}

int devices::archon::get_states()
{
  return std::stoi(readKeyValue("STATES"));

}

void devices::archon::set_states(int n)
{
  writeKeyValue("STATES",std::to_string(n));
}

int devices::archon::get_constants()
{
  return std::stoi(readKeyValue("CONSTANTS"));
}

void devices::archon::set_constants(int n)
{
  writeKeyValue("CONSTANTS",std::to_string(n));
}

int devices::archon::get_parameters()
{
  return std::stoi(readKeyValue("PARAMETERS"));

}

void devices::archon::set_parameters(int n)
{
  writeKeyValue("PARAMETERS",std::to_string(n));
}


void foxtrot::devices::archon::set_power(bool onoff)
{
    if(onoff)
    {
        cmd("POWERON");
    }
    else
    {
        cmd("POWEROFF");
    }
    
}

void foxtrot::devices::archon::load_timing_script(const std::string& script)
{
    std::stringstream ss;
    std::string to;
    
    int i=0;
    while(std::getline(ss,to,'\n'))
    {
       auto configkey = "LINE" + std::to_string(i++); 
       auto line = "\"" + to + "\"";
       writeKeyValue(configkey,line);
    };
    
    set_timing_lines(i);
    
}



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::archon;
 
 //TODO: fetch all logs
 
 registration::class_<archon>("foxtrot::devices::archon")
 .method("clear_config",&archon::clear_config)
 .property_readonly("fetch_log",&archon::fetch_log)
 .method("update_state",&archon::update_state)
 .method("applyall",&archon::applyall)
 .method("set_power",&archon::set_power)
 ;
    
    
    
}

