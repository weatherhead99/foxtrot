#include "archon.h"
#include "archon_modules.h"
#include "archon_module_heaterx.h"
#include "archon_module_lvxbias.h"
#include "archon_module_hvxbias.h"
#include "archon_module_AD.h"
#include "archon_module_xvbias.h"
#include "archon_module_driver.h"
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
    
    std::unique_ptr<ArchonModule> ptr(nullptr);
    switch(modtype)
    {
        case(archon_module_types::HeaterX):
	  _lg.Info("HeaterX module detected at position " + std::to_string(i+1));
        ptr = ArchonHeaterX::constructModule(*this,i);
        break;
	  
	  
	case(archon_module_types::LVXBias):
	  _lg.Info("LVXBias module detected at position " + std::to_string(i+1));
	  ptr = ArchonLVX::constructModule(*this,i);
	  break;
	
	case(archon_module_types::HVXBias):
	  _lg.Info("HVXBias module detected at position " + std::to_string(i+1));
	  ptr = ArchonHVX::constructModule(*this,i);
	  break;
	  
	case(archon_module_types::AD):
	  _lg.Info("A/D module detected at position " + std::to_string(i+1));
	  ptr = ArchonAD::constructModule(*this,i);
	  break;
      
    case(archon_module_types::XVBias):
        _lg.Info("XVBias module detected at position " + std::to_string(i+1));
        ptr = ArchonXV::constructModule(*this,i);
        break;
        
    case(archon_module_types::Driver):
        _lg.Info("Driver module detected at position " + std::to_string(i+1));
        ptr =  ArchonDriver::constructModule(*this,i);
        break;
	  
	default:
	  _lg.Info("support for module at position " + std::to_string(i+1)  + " isn't implemented yet");
	    
    };
    
    
    if(ptr)
    {
        _modules.insert(std::make_pair(i,std::move(ptr)));
    }
    
    
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
  _configlinemap.clear();

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

bool devices::archon::get_power()
{
  auto power = std::stoi(getStatus().at("POWER"));
  switch(power)
  {
    case(0): _lg.Error("unknown power state");
    throw DeviceError("unknown power state");
    case(1): _lg.Error("no configuration applied");
    throw DeviceError("no configuration applied");
    case(2): return false;
    case(3): _lg.Error("intermediate power");
    throw DeviceError("intermediate power state");
    case(4): return true;
    case(5): return false;
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
    
    cmd("LOADTIMING");
    
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
    int confnum;
    try{
        confnum = _constantmap.at(name);
    }
    catch(std::out_of_range& err)
    {   
        confnum = get_constants();
        _constantmap[name] = confnum;
        set_constants(confnum +1);
    }
    
    std::ostringstream oss;
    oss << "CONSTANT" << confnum;
    
  writeKeyValue(oss.str(),std::to_string(val));
}


unsigned int devices::archon::getParam(const string& name)
{
    int confnum;
    try{
        confnum = _constantmap.at(name);
    }
    catch(std::out_of_range& err)
    {
        throw DeviceError("parameter not known to this archon");
    }
    
    std::ostringstream oss;
    oss << "PARAMETER" << confnum;
    
  return std::stoul(readKeyValue(oss.str()));

}


void devices::archon::setParam(const string& name, unsigned int val)
{
    int confnum;
    try{
        confnum = _parammap.at(name);
    }
    catch(std::out_of_range& err)
    {   
        confnum = get_parameters();
        _parammap[name] = confnum;
        set_parameters(confnum +1);
    }
    
    std::ostringstream oss;
    oss << "PARAMETER" << confnum;
    
  writeKeyValue(oss.str(),std::to_string(val));
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

void devices::archon::settapline(int n, const string& tapline)
{
  if(n > _taplines )
  {
    throw DeviceError("invalid TAP line number");
  }
  
  std::ostringstream oss;
  oss << "TAPLINE" << n;
  
  writeKeyValue(oss.str(),tapline);
  _taplines++;
  
  writeKeyValue("TAPLINES", std::to_string(_taplines +1));

  
  
  
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


int devices::archon::get_frameno(int buf)
{
    std::ostringstream oss ;
  oss << "BUF" << buf << "FRAME";
  return std::stoi(_frame.at(oss.str()));


}

int devices::archon::get_height(int buf)
{
    std::ostringstream oss ;
  oss << "BUF" << buf << "HEIGHT";
  return std::stoi(_frame.at(oss.str()));


}

int devices::archon::get_width(int buf)
{
  std::ostringstream oss ;
  oss << "BUF" << buf << "WIDTH";
  return std::stoi(_frame.at(oss.str()));

}

bool devices::archon::isbuffercomplete(int buf)
{
  std::ostringstream oss;
  oss << "BUF" << buf << "COMPLETE";
  auto complete = _frame.at(oss.str());
  
  return std::stoi(complete);

}

int devices::archon::get_mode(int buf)
{
    std::ostringstream oss;
  oss << "BUF" << buf << "MODE";
  auto complete = _frame.at(oss.str());
  
  return std::stoi(complete);

}

bool devices::archon::get_32bit(int buf)
{
      std::ostringstream oss;
  oss << "BUF" << buf << "SAMPLE";
  auto complete = _frame.at(oss.str());
  
  return std::stoi(complete);


}

void devices::archon::setrawchannel(int ch)
{
  writeKeyValue("RAWSEL",std::to_string(ch));

}
void devices::archon::setrawenable(bool onoff)
{
  writeKeyValue("RAWENABLE",std::to_string(onoff));

}

void devices::archon::setrawendline(int line)
{
  writeKeyValue("RAWENDLINE", std::to_string(line));

}
void devices::archon::setrawsamples(int n)
{
  writeKeyValue("RAWSAMPLES", std::to_string(n));

}

void devices::archon::setrawstartline(int line)
{
  writeKeyValue("RAWSTARTLINE",std::to_string(line));

}
void devices::archon::setrawstartpixel(int pix)
{
  writeKeyValue("RAWSTARTPIXEL",std::to_string(pix));

}





std::vector< unsigned int > devices::archon::fetch_buffer(int buf)
{
  std::vector<unsigned int> out;
  
  //need to update state or width/height might actually be wrong
  update_state();
  
  lockbuffer(buf);
  
  if(!isbuffercomplete(buf))
  {
    throw DeviceError("buffer not complete for reading!");
  }
  
  auto pixels = get_width(buf) * get_height(buf);
  out.reserve(pixels);
  
  std::ostringstream oss;
  oss << "BUF" << buf << "BASE";
  auto baseaddr = std::stoul(_frame.at(oss.str()),0,16);
 
  unsigned num_bytes = get_32bit(buf) ? pixels * 4 : pixels * 2;
  
  auto num_blocks = num_bytes / 1024 ;
  num_blocks = (num_bytes % 1024 == 0) ? num_blocks: num_blocks + 1;
  
  
  
  oss.str("");
  oss << ">00FETCH" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << baseaddr << num_blocks <<'\n';
  
  //construct command manually
  _specproto->write(oss.str());
  bool is32 = get_32bit(buf);
  
  for(int i=0; i < num_blocks; i++)
  {
    unsigned actlen;
    auto ret = _specproto->read(1024,&actlen);
    if(actlen != 1024)
    {
      _lg.Warning("cycle: " + std::to_string(i));
      _lg.Warning("didn't read 1024 bytes...");
      
    }
    if(ret[0] != '<' )
    {
      _lg.Error("got RET: " + ret );
      _lg.Error("request was: " + oss.str());
      if(ret[0] == '?')
      {
      throw DeviceError("archon threw an error message! Check the logs..."); 
      }  
      throw ProtocolError("invalid archon response!");
    };
    
   auto bytes = parse_binary_response(ret.substr(3));
   
   if(is32)
   {
      unsigned* ptr = reinterpret_cast<unsigned*>(bytes.data());
      out.insert(out.end(),ptr, ptr + 1024 / 4);
   }
   else
   {
     //WARNING: does this do the stride properly?
     unsigned short* ptr = reinterpret_cast<unsigned short*>(bytes.data());
     out.insert(out.end(),ptr, ptr + 1024 / 2);
     
   }
    
  };
  
  return out;
  
  
}

void foxtrot::devices::archon::setCDSTiming(int reset_start, int reset_end, int signal_start, int signal_end)
{
    writeKeyValue("SHP1",std::to_string(reset_start));
    writeKeyValue("SHP2",std::to_string(reset_end));
    writeKeyValue("SHD1", std::to_string(signal_start));
    writeKeyValue("SHD2", std::to_string(signal_end));
    
    cmd("APPLYCDS");
    
}

int foxtrot::devices::archon::getreset_start()
{
    auto val = readKeyValue("SHP1");
    return std::stoi(val);
}

int foxtrot::devices::archon::getreset_end()
{
    auto val = readKeyValue("SHP2");
    return std::stoi(val);
}

int foxtrot::devices::archon::getsignal_start()
{
    auto val = readKeyValue("SHD1");
    return std::stoi(val);
}

int foxtrot::devices::archon::getsignal_end()
{
    auto val = readKeyValue("SHD2");
    return std::stoi(val);
}


void devices::archon::settrigoutinvert(bool invert)
{
  writeKeyValue("TRIGOUTINVERT",std::to_string((int) invert));

}

void devices::archon::settrigoutpower(bool onoff)
{
  writeKeyValue("TRIGOUTPOWER", std::to_string((int) onoff));

}


void devices::archon::setframemode(int mode)
{
  writeKeyValue("FRAMEMODE", std::to_string(mode));

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
 .method("get_frameno", &archon::get_frameno)
 (parameter_names("buf"))
 .method("get_width", &archon::get_width)
 (parameter_names("buf"))
 .method("get_height",&archon::get_height)
 (parameter_names("buf"))
 .method("get_mode",&archon::get_mode)
 (parameter_names("buf"))
 .method("get_32bit",&archon::get_32bit)
 (parameter_names("buf"))
 .method("fetch_buffer",&archon::fetch_buffer)
 (parameter_names("buf"), metadata("streamdata",true))
 .property_readonly("getreset_start",&archon::getreset_start)
 .property_readonly("getreset_end",&archon::getreset_end)
 .property_readonly("getsignal_start",&archon::getsignal_start)
 .property_readonly("getsignal_end",&archon::getsignal_end)
 .method("setCDSTiming",&archon::setCDSTiming)
 (parameter_names("reset_start","reset_end","signal_start","signal_end"))
 .method("settapline", &archon::settapline)
 (parameter_names("n","tapline"))
 .method("settrigoutinvert", &archon::settrigoutinvert)
 (parameter_names("invert"))
 .method("settrigoutpower", &archon::settrigoutpower)
 (parameter_names("onoff"))
 .method("setrawenable",&archon::setrawenable)
 (parameter_names("onoff"))
 .method("setrawchannel",&archon::setrawchannel)
 (parameter_names("ch"))
 .method("setrawstartline",&archon::setrawstartline)
 (parameter_names("line"))
 .method("setrawendline",&archon::setrawendline)
 (parameter_names("line"))
 .method("setrawstartpixel",&archon::setrawstartpixel)
 (parameter_names("pix"))
 .method("setrawsamples", &archon::setrawsamples)
 (parameter_names("n"))
 .method("setframemode", &archon::setframemode)
 (parameter_names("mode"))
 .property_readonly("get_timing_lines",&archon::get_timing_lines)
 .property_readonly("get_states",&archon::get_states)
 .property_readonly("get_constants",&archon::get_constants)
 .property_readonly("get_power",&archon::get_power)
 .property_readonly("get_parameters",&archon::get_parameters)
 ;
    
    
}

