#include "archon.h"
#include "archon_modules.h"
#include "archon_module_heaterx.h"
#include "archon_module_lvxbias.h"
#include "archon_module_hvxbias.h"
#include "archon_module_AD.h"
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
    _order(0), _lg("archon")
{
  proto->Init(nullptr);
  
  //NOTE: this used to clear existing config
  //TODO: still not reliable
  
//   read_parse_existing_config();
  clear_config();
  

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
	  _lg.Info("HeaterX module detected at position " + std::to_string(i+1));
            ptr = ArchonHeaterX::constructModule(*this,i);
            _modules.insert(std::pair<int,std::unique_ptr<ArchonModule>>(i, std::move(ptr)));
            break;
	  
	  
	case(archon_module_types::LVXBias):
	  _lg.Info("LVXBias module detected at position " + std::to_string(i+1));
	  ptr = ArchonLVX::constructModule(*this,i);
	  _modules.insert(std::make_pair(i,std::move(ptr)));
	  break;
	
	case(archon_module_types::HVXBias):
	  _lg.Info("HVXBias module detected at position " + std::to_string(i+1));
	  ptr = ArchonHVX::constructModule(*this,i);
	  _modules.insert(std::make_pair(i,std::move(ptr)));
	  break;
	  
	case(archon_module_types::AD):
	  _lg.Info("A/D module detected at position " + std::to_string(i+1));
	  ptr = ArchonAD::constructModule(*this,i);
	  _modules.insert(std::make_pair(i,std::move(ptr)));
	  break;
	  
	  
	default:
	  _lg.Info("support for module at position " + std::to_string(i+1)  + " isn't implemented yet");
	    
    };
    
    
   }
   else
   {
     std::cout << "module not present at position " << (i+1) << std::endl;
   };
   
   
   
  }
  
  
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

std::vector<unsigned char> foxtrot::devices::archon::parse_binary_response(const std::string& response)
{
    std::vector<unsigned char> out;
    out.reserve(1024);
    
    if(response[0] != ':')
    {
        throw ProtocolError("expected a binary response but didn't get one!");
    }
    
    out.assign(response.begin() +1, response.end());
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

const ssmap & foxtrot::devices::archon::getFrame() const
{
    return _frame;
}


void devices::archon::update_state()
{
  
  std::cout << "system.." << std::endl;
  _system = parse_parameter_response(cmd("SYSTEM"));
  std::cout << "status.." << std::endl;
  _status = parse_parameter_response(cmd("STATUS"));
  std::cout << "frame.." << std::endl;
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
    throw std::logic_error("trying to overwrite a config line " + std::to_string(num) + " that doesn't exist yet");
  };
  
  std::ostringstream oss;
  oss << "WCONFIG" << std::setw(4) << std::setfill('0') << std::uppercase<<  std::hex << num << line;
  cmd(oss.str());
  
  return num;

}



std::string devices::archon::readConfigLine(int num, bool override_existing)
{
  if( num > _config_lines && !override_existing)
  {
    throw std::logic_error("tried to read an archon config line that we don't think exists...");
  };
  
  std::ostringstream oss;
  oss << "RCONFIG" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << num ;
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
    std::stringstream ss(script);
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




void foxtrot::devices::archon::lockbuffer(int buf)
{
    if(buf < 1 || buf > 3)
    {
        throw std::out_of_range("invalid buffer, must be 1 :3 ");
    }
    
    cmd("LOCK" + std::to_string(buf));
}


void foxtrot::devices::archon::unlockbuffers()
{
    cmd("LOCK0");
}

void foxtrot::devices::archon::read_parse_existing_config()
{
    //NOTE: need to write at least one config line because reading empty config results in infinite loop
    
    writeKeyValue("RAWSTARTPIXEL","0");
    
    int i;
    for(i =0 ; i < 2048; i++)
    {
        auto confline = readConfigLine(i,true);
        if(confline.size() == 0)
        {
            break;
        }
        
        auto eqpos = std::find(confline.begin(), confline.end(),'=');
        if(eqpos == confline.end())
        {
            throw DeviceError("malformed config line returned from archon");    
        };
  
        auto key = string(confline.begin(),eqpos);
        auto val = string(eqpos+1,confline.end());
  
        _configlinemap.insert({key,i});
        
    }
    
    _config_lines = i;
    
    
    
}

void foxtrot::devices::archon::holdTiming()
{
    cmd("HOLDTIMING");
}

void foxtrot::devices::archon::releaseTiming()
{
    cmd("RELEASETIMING");
}

void foxtrot::devices::archon::resetTiming()
{
    cmd("RESETTIMING");
}

unsigned int devices::archon::getConstant(const string& name)
{
  return std::stoul(readKeyValue(name));
  
}

void devices::archon::setConstant(const string& name, unsigned int val)
{
  try
  {
    readKeyValue(name);
  }
  catch(std::runtime_error& err)
  {
   //NOTE: key is not in registry 
    set_constants(get_constants()  +1);
  }
  
  writeKeyValue(name,std::to_string(val));
}


unsigned int devices::archon::getParam(const string& name)
{
  return std::stoul(readKeyValue(name));

}


void devices::archon::setParam(const string& name, unsigned int val)
{
  bool newparam = false;
  try
  {
    readKeyValue(name);
  }
  catch(std::runtime_error& err)
  {
   //NOTE: key is not in registry 
    newparam = true;
  }
  
  writeKeyValue(name,std::to_string(val));

  //NOTE: do this last for exception safety
  if(newparam)
  {
    set_parameters(get_parameters() + 1);
  }
  
  
}

void devices::archon::write_timing_state(const string& name, const string& state)
{
  std::stringstream ss(state);
  std::string to;
  bool newstate = ( std::find(_statenames.begin(), _statenames.end(), name)  == _statenames.end() );
  
  //NOTE: difference of 1 between index and number total
  int this_state = newstate? get_states() : get_states() -1;
  
  
  while(std::getline(ss,to,'\n'))
  {
     auto eqpos = std::find(to.begin(),to.end(),'=');
     if(eqpos == to.end())
     {
       throw std::runtime_error("malformed timing state line: " + to);
     }
     
     std::string configkey(to.begin(),eqpos);
     configkey = "STATE" + std::to_string(this_state) + "/" + configkey; 
     std::string val(eqpos + 1, to.end());
     
     writeKeyValue(configkey, val);
  }
  
  //NOTE: do this now in case of exceptions
  if(newstate)
  {
    set_states(get_states() + 1);
  }
   

}

void devices::archon::apply_all_params()
{
  cmd("LOADPARAMS");

}

void devices::archon::apply_param(const string& name)
{
  cmd("LOADPARAM " + name);

}

void foxtrot::devices::archon::sync_archon_timer()
{
    _arch_tmr = std::stoull(cmd("TIMER"));
    _sys_tmr = boost::posix_time::microsec_clock::universal_time();
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
 .method("load_timing_script", &archon::load_timing_script)
 .method("getParam", &archon::getParam)
 (
   parameter_names("name")
 )
 .method("setParam",&archon::setParam)
 (
   parameter_names("name","val")
   )
 .method("getConstant",&archon::getConstant)
 (
   parameter_names("name")
   )
 .method("setConstant", &archon::setConstant)
 (
   parameter_names("name","val")
   )

 .method("apply_param", &archon::apply_param)
 (
   parameter_names("name")
   )
 .method("apply_all_params", &archon::apply_all_params)
 .method("holdTiming", &archon::holdTiming)
 .method("releaseTiming",&archon::releaseTiming)
 .method("resetTiming", &archon::resetTiming)
 .method("lockbuffer",&archon::lockbuffer)
 (
   parameter_names("buf")
   )
 .method("unlockbuffers",&archon::unlockbuffers)
 .method("write_timing_state",&archon::write_timing_state)
 (
   parameter_names("name", "state")
   )
 ;
 
    
    
    
}

