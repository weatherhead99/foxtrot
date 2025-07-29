#include <algorithm>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <locale>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

#include <foxtrot/DeviceError.h>
#include <foxtrot/ProtocolError.h>
#include <foxtrot/ft_tuple_helper.hh>


#include <foxtrot/protocols/ProtocolUtilities.h>
#include <foxtrot/protocols/CommunicationProtocol.h>

#include "archon.h"
#include "archon_modules.h"
#include "archon_module_heaterx.h"
#include "archon_module_lvxbias.h"
#include "archon_module_hvxbias.h"
#include "archon_module_AD.h"
#include "archon_module_xvbias.h"
#include "archon_module_driver.h"

#include "archon_module_mapper.hh"


#define READ_SIZE 1024

#define ARCHON_MAX_CONFIG_LINES 16384

using foxtrot::devices::archon;
using foxtrot::devices::ArchonStreamHelper;
using foxtrot::devices::archon_legacy;
using foxtrot::protocols::simpleTCPBase;

ArchonStreamHelper::ArchonStreamHelper(archon& dev) : _dev(dev) {}


struct foxtrot::devices::detail::archonimpl
{
  std::unordered_map<std::string, std::pair<std::string, int>> parammap;
  std::unordered_map<std::string, std::pair<std::string, int>> constvals;

};



const string devices::archon::getDeviceTypeName() const
{
  return "archon";
}

std::shared_ptr<archon> foxtrot::devices::archon::create(std::shared_ptr<simpleTCPBase>&&  proto, bool clear_config)
{
  std::shared_ptr<archon> out(new archon(proto));
  if(clear_config)
    out->clear_config();
  else
    out->read_parse_existing_config();
  out->setup_modules();

  return out;
};


foxtrot::devices::archon::archon(std::shared_ptr< foxtrot::protocols::simpleTCPBase > proto)
  : CmdDevice(std::static_pointer_cast<foxtrot::CommunicationProtocol>(proto)), _specproto(proto),
    _lg("archon"), _order(0)
{
  _impl = std::make_unique<detail::archonimpl>();
  proto->Init(nullptr);
  
  sync_archon_timer();
}

foxtrot::devices::archon::~archon()
{
  
}



std::string foxtrot::devices::archon::cmd(const std::string& request)
{
  std::unique_lock<std::mutex> lck(_cmdmut);
  if(_order == 0xFE)
    _order = 0;
  
  auto thisorder = _order;
  
  std::ostringstream oss;
  oss << ">" <<std::uppercase << std::hex << std::setw(2)<< std::setfill('0') << _order++ << request << "\n";

  _specproto->write(oss.str());    
  auto ret = _specproto->read_until_endl();
  lck.unlock();

  //sanitize response
  ret.erase(std::remove_if(ret.begin(),ret.end(),[] (char c) { return !std::isprint(c); }),ret.end());

  if(ret[0] != '<' )
  {

    _lg.Error("got RET: " + ret);
    _lg.Error("request was: " + request);
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
      _lg.Warning("ignoring problematic item: _" + item + "_" );
      continue;
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
    
    if(response[0] != '<' )
    {
      
      _lg.Error("invalid archon binary response!");
    
   
    if(response[0] == '?')
    {
      
     throw DeviceError("archon threw an error message! Check the logs..."); 
    }
    
    throw ProtocolError("invalid archon response!");
    };
  
    
    if(response[3] != ':')
    {
        throw ProtocolError("expected a binary response but didn't get one!");
    }
    
    out.assign(response.begin() +4, response.end());
    return out;
    
}



ssmap devices::archon::getStatus()
{
  return parse_parameter_response(cmd("STATUS"));
}

ssmap devices::archon::getSystem()
{
  return parse_parameter_response(cmd("SYSTEM"));
}

ssmap foxtrot::devices::archon::getFrame()
{
  return parse_parameter_response(cmd("FRAME"));
}

foxtrot::devices::archon_status archon::status()
{
  archon_status out;
  //this is a string to string map
  auto statmap = getStatus();

  _lg.strm(sl::trace) << "parsing basic system things...";
  out.valid = std::stoi(statmap.at("VALID"));
  out.count = std::stoi(statmap.at("COUNT"));
  out.nlogs = std::stoi(statmap.at("LOG"));
  out.powerstatus = static_cast<archon_power_status>(std::stoul(statmap.at("POWER")));
  out.powergood = std::stoi(statmap.at("POWERGOOD"));
  out.overheat = std::stoi(statmap.at("OVERHEAT"));
  out.backplane_temp = std::stod(statmap.at("BACKPLANE_TEMP"));

  _lg.strm(sl::trace) << "parsing PSU voltages and currents...";
  std::array<string,10> voltagestrs = {"P2V5", "P5V", "P6V", "N6V", "P17V", "N17V", "P35V", "N35V", "P100V", "N100V"};

  for(auto vstr: voltagestrs)
    {
      double V = std::stod(statmap.at(std::format("{}_V",vstr)));
      double I = std::stod(statmap.at(std::format("{}_I", vstr)));
      out.PSU_map.emplace(vstr, std::make_pair(V, I));
    }

  
  double user_I = std::stod(statmap.at("USER_I"));
  double user_V = std::stod(statmap.at("USER_V"));
  double heater_I = std::stod(statmap.at("HEATER_I"));
  double heater_V = std::stod(statmap.at("HEATER_V"));

  out.PSU_map.emplace("USER", std::make_pair(user_V, user_I));
  out.PSU_map.emplace("HEATER", std::make_pair(heater_V, heater_I));
  
  if(statmap.contains("FANTACH"))
    out.fanspeed = std::stoul(statmap.at("FANTACH"));

  _lg.strm(sl::trace) << "calling status() for each module";
  out.module_statuses.reserve(_modules.size());
  for(auto& [pos, mod] : _modules)
    out.module_statuses.push_back(mod->status(statmap));
  
  return out;
}

foxtrot::devices::archon_frame_info archon::frameinfo()
{
  archon_frame_info out;
  auto framemap = getFrame();
  out.rbuf = std::stoul(framemap.at("RBUF"));
  out.wbuf = std::stoul(framemap.at("WBUF"));

  auto bufit = out.buffer_infos.begin();
  for(int i=1; i<=3; i++)
    {
      auto gs = [&framemap, i, this](const string& txt)
      {
	string req = std::format("BUF{}{}", i, txt);
	_lg.strm(sl::trace) << "key formatted:  " << req;
	return framemap.at(req);
      };
      
      bufit->sampmode = static_cast<archon_sample_mode>(std::stoi(gs("SAMPLE")));
      bufit->complete = std::stoi(gs("COMPLETE"));
      bufit->bufmode = static_cast<archon_buffer_mode>(std::stoi(gs("MODE")));
      bufit->offsetaddr = std::stoul(gs("BASE"));
      bufit->frame_number = std::stoul(gs("FRAME"));
      bufit->width = std::stoul(gs("WIDTH"));
      bufit->height = std::stoul(gs("HEIGHT"));
      bufit->pixel_progress = std::stoul(gs("PIXELS"));
      bufit->line_progress = std::stoul(gs("LINES"));
      bufit->raw_blocks = std::stoul(gs("RAWBLOCKS"));
      bufit->raw_lines = std::stoul(gs("RAWLINES"));
      bufit->raw_offset = std::stoul(gs("RAWOFFSET"));
      bufit->timestamp = archon_time_to_real_time(std::stoull(gs("TIMESTAMP"), nullptr, 16));
      bufit->REtimestamp = archon_time_to_real_time(std::stoull(gs("RETIMESTAMP"), nullptr, 16));
      bufit->FEtimestamp = archon_time_to_real_time(std::stoull(gs("FETIMESTAMP"), nullptr, 16));
      bufit->REAtimestamp = archon_time_to_real_time(std::stoull(gs("REATIMESTAMP"), nullptr, 16));
      bufit->FEAtimestamp = archon_time_to_real_time(std::stoull(gs("FEATIMESTAMP"), nullptr, 16));
      bufit->REBtimestamp = archon_time_to_real_time(std::stoull(gs("REBTIMESTAMP"), nullptr, 16));
      bufit->FEBtimestamp = archon_time_to_real_time(std::stoull(gs("FEBTIMESTAMP"), nullptr, 16));

      ++bufit;
    }
  
  return out;
}

foxtrot::devices::archon_system_info archon::system()
{
  archon_system_info out;

  auto sysmap = getSystem();

  _lg.strm(sl::trace) << "printing system map:";
  for(const auto& [k, v] : sysmap)
    {
      _lg.strm(sl::trace) << k << ": " << v ;
    }
  
   _lg.strm(sl::trace) << "backplane type...";
   out.backplane_type = std::stoul(sysmap.at("BACKPLANE_TYPE"));
   _lg.strm(sl::trace) << "type is: " << out.backplane_type;
  _lg.strm(sl::trace) << "backplane rev...";
  out.backplane_rev = std::stoul(sysmap.at("BACKPLANE_REV"));
  _lg.strm(sl::trace) << "rev is: " << out.backplane_rev;
  out.backplane_version = sysmap.at("BACKPLANE_VERSION");
  _lg.strm(sl::trace) << "backplane version is: " << out.backplane_version;
  out.backplane_id = std::stoull(sysmap.at("BACKPLANE_ID"), nullptr, 16);
  out.power_id = std::stoull(sysmap.at("POWER_ID"), nullptr, 16);
  std::vector<unsigned short> mod_positions;

  auto mod_positions_setup = [&mod_positions, &sysmap, this] <int N> () {
    unsigned modflagmap = std::stoul(sysmap.at("MOD_PRESENT"), nullptr, 16);
    _lg.strm(sl::trace) << "modflagmap: " << modflagmap;
      std::bitset<N> modpos = modflagmap;
      auto nmods = modpos.count();
      mod_positions.reserve(nmods);
      for(int i=0; i< N; i++)
	{
	  if(modpos.test(i))
	    mod_positions.push_back(i+1);
	}
  };
  
  if(out.backplane_type == 1)
    mod_positions_setup.template operator()<12>();
  else if(out.backplane_type == 2)
    mod_positions_setup.template operator()<16>();

  out.modules.reserve(mod_positions.size());
  for(auto pos : mod_positions)
    {
      _lg.strm(sl::trace) << "looking for module info at position: " << pos;
      archon_module_info& modinfo = out.modules.emplace_back();
      auto modidstr = std::format("MOD{}_ID", pos);
      _lg.strm(sl::trace) << "modidstr: " << modidstr;
      modinfo.module_id = std::stoull(sysmap.at(modidstr), nullptr, 16);
      _lg.strm(sl::trace) << "module id: " << modinfo.module_id;
      modinfo.position = pos;
      modinfo.version = sysmap.at(std::format("MOD{}_VERSION", pos));
      _lg.strm(sl::trace) << "module version: " << modinfo.version;
      modinfo.revision = std::stoul(sysmap.at(std::format("MOD{}_REV", pos)));
      _lg.strm(sl::trace) << "module revision: " << modinfo.revision;
      modinfo.type = static_cast<archon_module_types>(std::stoul(sysmap.at(std::format("MOD{}_TYPE", pos))));
    }

  return out;
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
  _configlinemap.clear();
  _statenames.clear();
  _parammap.clear();
  _constantmap.clear();
  
  _ADtaplinemap.clear();
  _taplines = 0;
  _states=  0;
  
    //setup lines, timing lines etc...
  set_timing_lines(0);
  set_states(0);
  set_parameters(0); 
  set_constants(0);
  writeKeyValue("TAPLINES","0");
  
}


int devices::archon::writeConfigLine(const string& line,int num)
{
  //check if possible
  if(_configlinemap.size() >= ARCHON_MAX_CONFIG_LINES)
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
    num = _configlinemap.size();
  }
  else if(num > _configlinemap.size())
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
  if( num >= _configlinemap.size() && !override_existing)
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
  int nLogs = status().nlogs;
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


std::unordered_map<string, int> devices::archon::parameters()
{
  std::unordered_map<string, int> out;
  auto n_params = get_parameters();
  for(int i=0; i< n_params; i++)
    {
      
      out.emplace_back({
    }
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
  
  _lg.strm(sl::error) << "unrecognized power state: " << power;
  throw std::logic_error("unknown power state in reply");

}



void foxtrot::devices::archon::load_timing_script(const std::string& script)
{
    std::stringstream ss(script);
    std::string to;
    
    int i=0;
    while(std::getline(ss,to,'\n'))
    {
       auto configkey = "LINE" + std::to_string(i++); 
//     how about that ?   
//        auto line = "\"" + to + "\"";
       auto line = to;
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
  auto stat = status();
  if(stat.powerstatus == foxtrot::devices::archon_power_status::not_configured)
    throw foxtrot::DeviceError("no configuration loaded in archon, cannot parse!");
    _configlinemap.clear();
    int i;
    for(i =0 ; i < ARCHON_MAX_CONFIG_LINES; i++)
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

double devices::archon::getConstant(const string& name)
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
    oss << "CONSTANT" << confnum;
    auto paramline = readKeyValue(oss.str());
    
    auto eqpos = std::find(paramline.begin(),paramline.end(),'=');
    auto quotepos = std::find(eqpos, paramline.end(),'\"');
    
    //TODO: this will fail at runtime
    return std::stod(std::string(eqpos+1,quotepos));

  
}

void devices::archon::setConstant(const string& name, double val)
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
    
    std::ostringstream osskey;
    osskey << "CONSTANT" << confnum;
    
    std::ostringstream ossval;
    ossval << name << '=' << val ;
    
  writeKeyValue(osskey.str(),ossval.str());
}


unsigned int devices::archon::getParam(const string& name)
{
    int confnum;
    try{
        confnum = _parammap.at(name);
    }
    catch(std::out_of_range& err)
    {
        throw DeviceError("parameter not known to this archon");
    }
    
    std::ostringstream oss;
    oss << "PARAMETER" << confnum;
    auto paramline = readKeyValue(oss.str());
    auto eqpos = std::find(paramline.begin(),paramline.end(),'=');
    auto quotepos = std::find(eqpos, paramline.end(),'\"');
    //TODO: this will fail at runtime
    return std::stoul(std::string(eqpos+1,quotepos));

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
    
    std::ostringstream osskey;
    osskey << "PARAMETER" << confnum;
    
    std::ostringstream ossval;
    ossval << name << '=' << val ;
    
  writeKeyValue(osskey.str(),ossval.str());
}

void devices::archon::write_timing_state(string name, const string& state)
{
  std::stringstream ss(state);
  std::string to;
  auto findstate = std::find(_statenames.begin(),_statenames.end(),name);
  auto state_idx = std::distance(_statenames.begin(),findstate);
  
  bool newstate = ( findstate  == _statenames.end() );
  
  
  while(std::getline(ss,to,'\n'))
  {
     auto eqpos = std::find(to.begin(),to.end(),'=');
     if(eqpos == to.end())
     {
       throw std::runtime_error("malformed timing state line: " + to);
     }
     
     std::string configkey(to.begin(),eqpos);
     configkey = "STATE" + std::to_string(state_idx) + "/" + configkey; 
     std::string val(eqpos + 1, to.end());
     
     writeKeyValue(configkey, val);
  }
  
  //NOTE: do this now in case of exceptions
  if(newstate)
  {
    set_states(_statenames.size());
   _statenames.insert(_statenames.end(),name);
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
  
  if(n == _taplines)
  {
    _taplines++;
    writeKeyValue("TAPLINES", std::to_string(_taplines ));
  };
  
}

std::string assemble_tapline(const string& defn, unsigned char AD, bool LR, double gain, double offset)
{
  char LRchar = LR ? 'R' : 'L';
  auto taplinestr = std::format("{}{},{},{}", defn, LRchar, gain, offset);
  return taplinestr;

}
			     

void foxtrot::devices::archon::settap(unsigned char AD, bool LR, double gain, unsigned short offset)
{
        // char LRchar = LR ? 'R' : 'L';
        // std::ostringstream oss;
        // oss << "AD" << static_cast<unsigned>(AD) << LRchar << ',' << gain << ',' << offset;
        
        //WARNING: all sorts of edge cases that could blow up later here
        //UPDATE (2025!!!).... yep, past me present me etc etc
  if(_using_AM_taps.value_or(false) == true)
    throw std::runtime_error("attempting to set an AD tap on an Archon already configured with AM taps!");

  auto taplinestr = assemble_tapline("AD", AD, LR, gain, offset);

	if(_ADtaplinemap.contains(AD))
	  {
	    int tline = _ADtaplinemap.at(AD);
	    settapline(tline, taplinestr);
	  }
	else {
	  _ADtaplinemap[AD] = _taplines;
	  settapline(_taplines, taplinestr);
	    }
	_using_AM_taps = false;
	
}

void foxtrot::devices::archon::setAMtap(unsigned char AD, bool LR, double gain, unsigned short offset)
{
  if(_using_AM_taps.value_or(true) == false)
    throw std::runtime_error("attempting to set an AM tap on an Archon already using AD taps!");
  auto taplinestr = assemble_tapline("AM",  AD,  LR, gain, offset);

  if(_ADtaplinemap.contains(AD))
    {
      int tline = _ADtaplinemap.at(AD);
      settapline(tline, taplinestr);
    }
  else {
    _ADtaplinemap[AD] = _taplines;
    settapline(_taplines, taplinestr);
      }

  _using_AM_taps = true;

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
    _lg.Info("syncing timer");
    auto str = cmd("TIMER");
    auto eqpos = std::find(str.begin(), str.end(),'=');
    std::string count(eqpos+1,str.end());
    
    _lg.strm(sl::debug) << "timerstr: '" << count << "'";
    
    _arch_tmr = std::stoull(count);
    _sys_tmr = std::chrono::high_resolution_clock::now();
}




void devices::archon::set_tap_pixels(short unsigned npixels)
{
  writeKeyValue("PIXELCOUNT", std::to_string(npixels));
  cmd("APPLYCDS");
}

short unsigned devices::archon::get_tap_pixels()
{

  return std::stoi(readKeyValue("PIXELCOUNT"));
}

void devices::archon::set_tap_lines(short unsigned lines)
{
  writeKeyValue("LINECOUNT",std::to_string(lines));
  cmd("APPLYCDS");

}

short unsigned devices::archon::get_tap_lines()
{
  return std::stoi(readKeyValue("LINECOUNT"));

}


void devices::archon::setrawchannel(int ch)
{
  writeKeyValue("RAWSEL",std::to_string(ch));
  cmd("APPLYCDS");
}

int foxtrot::devices::archon::getrawchannel()
{
    return std::stoi(readKeyValue("RAWSEL"));
}



void devices::archon::setrawenable(bool onoff)
{
  writeKeyValue("RAWENABLE",std::to_string((int) onoff));
  cmd("APPLYCDS");
}

bool foxtrot::devices::archon::getrawenable()
{
    return std::stoi(readKeyValue("RAWENABLE"));
}



void devices::archon::setrawendline(int line)
{
  writeKeyValue("RAWENDLINE", std::to_string(line));
  cmd("APPLYCDS");
}


int foxtrot::devices::archon::getrawendline()
{
    return std::stoi(readKeyValue("RAWENDLINE"));
}


void devices::archon::setrawsamples(int n)
{
  writeKeyValue("RAWSAMPLES", std::to_string(n));
  cmd("APPLYCDS");
}

int foxtrot::devices::archon::getrawsamples()
{
    return std::stoi(readKeyValue("RAWSAMPLES"));
}



void devices::archon::setrawstartline(int line)
{
  writeKeyValue("RAWSTARTLINE",std::to_string(line));
  cmd("APPLYCDS");
}

int foxtrot::devices::archon::getrawstartline()
{
    return std::stoi(readKeyValue("RAWSTARTLINE"));
}



void devices::archon::setrawstartpixel(int pix)
{
  writeKeyValue("RAWSTARTPIXEL",std::to_string(pix));
  cmd("APPLYCDS");
}


int foxtrot::devices::archon::getrawstartpixel()
{
    return std::stoi(readKeyValue("RAWSTARTPIXEL"));
}



template<typename T, typename Tdiff>
std::vector<T> devices::archon::read_back_buffer(int num_blocks, int retries, unsigned address)
{
    std::ostringstream oss;
    std::unique_lock<std::mutex> lck(_cmdmut);
    oss << ">00FETCH" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << address
    << std::setw(8) << std::setfill('0') << num_blocks << '\n';

    _specproto->write(oss.str());
    
    std::vector<T> out;
    out.reserve(num_blocks * 1024);
    
    unsigned actlen;
    
    for (int i = 0; i < num_blocks; i++)
    {
        
        for(int j =0; j < retries; j++)
        {
            auto bytes_avail = _specproto->bytes_available();
            if(bytes_avail >= 1028)
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(500));
            _lg.Debug("waiting for bytes available, currently:" + std::to_string(bytes_avail));
            
            if(j == (retries - 1) )
            {
	      throw foxtrot::DeviceError("ran out of retries waiting for buffer read, block:" + std::to_string(i));
            }
            
        }
        
        auto ret = _specproto->read(1028,&actlen);
        if(actlen != 1028)
        {
            _lg.Warning("didn't read 1028 bytes...");   
            _lg.Warning("read: " + std::to_string(actlen));
        }
        
        auto bytes = parse_binary_response(ret);
        
        Tdiff* ptr = reinterpret_cast<Tdiff*>(bytes.data());
        out.insert(out.end(),ptr, ptr + 1024 / sizeof(Tdiff));
           
    }
      
  _lg.Debug("read all blocks correctly");
  _lg.Debug("bytes still available: " + std::to_string(_specproto->bytes_available()));
  lck.unlock();
  _lg.Debug("size of output: " + std::to_string(out.size()));
  
    return out;
    
    
};


std::vector< unsigned int > devices::archon::fetch_buffer(int buf)
{
  std::vector<unsigned int> out;

  if(buf < 1 or buf > 3)
    throw std::out_of_range("buffer index supplied is not valid!");
  auto frameinfo = this->frameinfo();
  auto bufinfo = frameinfo.buffer_infos[buf -1];

  if(!bufinfo.complete)
  {
    throw DeviceError("buffer not complete for reading!");
  }

  lockbuffer(buf);
  auto pixels = bufinfo.width * bufinfo.height;
  bool is32 = bufinfo.sampmode == foxtrot::devices::archon_sample_mode::bit32;
  unsigned num_bytes = is32 ? pixels * 4 : pixels * 2;
  auto num_blocks = num_bytes / 1024  + (num_bytes % 1024 !=0) ;
  
  _lg.Debug("num_blocks: " + std::to_string(num_blocks));
  
  
  if(is32)
      out = read_back_buffer<unsigned>(num_blocks,100,bufinfo.offsetaddr);
  else
      out = read_back_buffer<unsigned, short unsigned>(num_blocks,100,bufinfo.offsetaddr + bufinfo.raw_offset);
  //drop extra bytes off the end
  out.resize(pixels);
  return out;
}

std::vector<unsigned short> foxtrot::devices::archon::fetch_raw_buffer(int buf)
{
    std::vector<unsigned short> out;
    if(buf < 1 or buf > 3)
      throw std::out_of_range("buffer index supplied is not valid!");
    auto frameinfo = this->frameinfo();
    auto bufinfo = frameinfo.buffer_infos[buf -1];    
    
    if(!bufinfo.complete)
        throw DeviceError("buffer not complete for reading!");
    lockbuffer(buf);
    
    //auto rawsamp = getrawsamples();
    auto rawsamp = bufinfo.raw_blocks;
    _lg.strm(sl::debug) << "raw blocks per line: " << rawsamp;
    auto rawlines = bufinfo.raw_lines;
    auto total_blocks = rawsamp  * rawlines ;
    _lg.strm(sl::debug) << "total blocks: " << total_blocks;
    out = read_back_buffer<unsigned short>(total_blocks,100, bufinfo.offsetaddr + bufinfo.raw_offset);
    
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
  cmd("APPLYSYSTEM");
}

void devices::archon::settrigoutpower(bool onoff)
{
  writeKeyValue("TRIGOUTPOWER", std::to_string((int) onoff));
  cmd("APPLYSYSTEM");
}

bool foxtrot::devices::archon::gettrigoutinvert()
{
    auto val = readKeyValue("TRIGOUTINVERT");
    return std::stoi(val);
}

bool foxtrot::devices::archon::gettrigoutpower()
{
    auto val = readKeyValue("TRIGOUTPOWER");
    return std::stoi(val);
}


bool devices::archon::gettrigoutforce()
{
  auto val = readKeyValue("TRIGOUTFORCE");
  return std::stoi(val);
}

void devices::archon::settrigoutforce(bool onoff)
{
  writeKeyValue("TRIGOUTFORCE", std::to_string((int) onoff));
  cmd("APPLYSYSTEM");
}

void devices::archon::settrigoutlevel(bool onoff)
{
  writeKeyValue("TRIGOUTLEVEL", std::to_string((int) onoff));
  cmd("APPLYSYSTEM");
}

bool devices::archon::gettrigoutlevel()
{
  auto val = readKeyValue("TRIGOUTLEVEL");
  return std::stoi(val);
}




void devices::archon::setframemode(int mode)
{
  writeKeyValue("FRAMEMODE", std::to_string(mode));
  cmd("APPLYCDS");
}

int foxtrot::devices::archon::getframemode()
{
    return std::stoi(readKeyValue("FRAMEMODE"));
}


void devices::archon::load_timing()
{
 cmd("LOADTIMING");   
}



void devices::archon::setup_modules()
{
    auto sys = system();
  int modules_installed = sys.modules.size();
  _lg.strm(sl::debug) << "modules installed: " << modules_installed;
  
  for(auto mod : sys.modules)
    {
      _lg.strm(sl::info) << "module found at position: " << mod.position;
      _lg.strm(sl::info) << "module  type is: " <<  get_module_name(mod.type);

      auto thisptr = std::static_pointer_cast<archon>(shared_from_this());
      auto ptr = make_module(thisptr, mod);
      if(ptr)
	{
	  _lg.strm(sl::info) << "module initialized";
	  _modules.insert(std::make_pair(mod.position, std::move(ptr)));
	}
      else
	_lg.strm(sl::warning) << "module failed to initialize (perhaps unimplemented)";
    }

}

foxtrot::devices::HRTimePoint devices::archon::archon_time_to_real_time(long long unsigned archon_time) const
{
  HRTimePoint out = _sys_tmr;
  auto ndiff_archon_ticks = archon_time - _arch_tmr;
  return out + (ndiff_archon_ticks * std::chrono::nanoseconds(10));
};


// --------------------ARCHON LEGACY CODE  STARTS HERER
// ---------------------------

std::shared_ptr<archon_legacy> foxtrot::devices::archon_legacy::create(std::shared_ptr<simpleTCPBase>&& proto)
{
  std::shared_ptr<archon_legacy> out(new archon_legacy(proto));
  out->setup_modules();
  return out;
}


foxtrot::devices::archon_legacy::archon_legacy(std::shared_ptr<simpleTCPBase> proto)
  : archon(proto) {}

foxtrot::devices::archon_legacy::~archon_legacy() {}

const string foxtrot::devices::archon_legacy::getDeviceTypeName() const
{
  return "archon";
}



void foxtrot::devices::archon_legacy::update_state()
{
  
  _lg.Trace("system..");
  _system = getSystem();
  _lg.Trace("status..");
  _status = getStatus();
  _lg.Trace("frame..");
  _frame = getFrame();

  //NOTE: need a replacement for this!
  
  // for(auto& mod: _modules)
  // {
  //   if(mod.second != nullptr)
  //   {
  //    mod.second->update_variables(); 
  //   }
  // }

}




bool foxtrot::devices::archon_legacy::isbuffercomplete(int buf)
{
  std::ostringstream oss;
  oss << "BUF" << buf << "COMPLETE";
  auto complete = _frame.at(oss.str());
  
  return std::stoi(complete);

}

int foxtrot::devices::archon_legacy::get_frameno(int buf)
{
    std::ostringstream oss ;
  oss << "BUF" << buf << "FRAME";
  return std::stoi(_frame.at(oss.str()));


}

int foxtrot::devices::archon_legacy::get_height(int buf)
{
    std::ostringstream oss ;
  oss << "BUF" << buf << "HEIGHT";
  return std::stoi(_frame.at(oss.str()));

}

int foxtrot::devices::archon_legacy::get_pixels(int buf)
{
      std::ostringstream oss ;
  oss << "BUF" << buf << "PIXELS";
    return std::stoi(_frame.at(oss.str()));

};


string foxtrot::devices::archon_legacy::get_tstamp(int buf)
{
  std::ostringstream oss;
  oss << "BUF" << buf << "TIMESTAMP";
  auto tstamp = std::stoul(_frame.at(oss.str()),0,16);

  auto archontdiff = tstamp - _arch_tmr;
  //one tick of counter is 10ns
  auto frametime = _sys_tmr + std::chrono::nanoseconds(archontdiff * 10);
  return std::format("{:%FT%T}", frametime);
}



int foxtrot::devices::archon_legacy::get_rawlines(int buf)
{
  std::ostringstream oss;
  oss << "BUF" << buf << "RAWLINES";
  return std::stoi(_frame.at(oss.str()));

}


int foxtrot::devices::archon_legacy::get_rawblocks(int buf)
{
   std::ostringstream oss;
  oss << "BUF" << buf << "RAWBLOCKS";
  return std::stoi(_frame.at(oss.str()));
}



int foxtrot::devices::archon_legacy::get_width(int buf)
{
  std::ostringstream oss ;
  oss << "BUF" << buf << "WIDTH";
  return std::stoi(_frame.at(oss.str()));

}


int foxtrot::devices::archon_legacy::get_mode(int buf)
{
    std::ostringstream oss;
  oss << "BUF" << buf << "MODE";
  auto complete = _frame.at(oss.str());
  
  return std::stoi(complete);

}

bool foxtrot::devices::archon_legacy::get_32bit(int buf)
{
      std::ostringstream oss;
  oss << "BUF" << buf << "SAMPLE";
  auto complete = _frame.at(oss.str());
  
  return std::stoi(complete);


}



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::archon;
 using foxtrot::devices::archon_legacy;


 using foxtrot::devices::archon_status;
 using foxtrot::devices::archon_frame_info;
 using foxtrot::devices::archon_system_info;
 using foxtrot::devices::archon_power_status;
 using foxtrot::devices::archon_module_status;
 using foxtrot::devices::archon_module_types;
 using foxtrot::devices::archon_module_info;
 using foxtrot::devices::archon_buffer_info;
 using foxtrot::devices::archon_sample_mode;
 using foxtrot::devices::archon_buffer_mode;
 

 registration::enumeration<archon_buffer_mode>("foxtrot::devices::archon_buffer_mode")
   (value("top", archon_buffer_mode::top),
    value("bottom", archon_buffer_mode::bottom),
    value("split", archon_buffer_mode::split));

 registration::enumeration<archon_sample_mode>
   ("foxtrot::devices::archon_sample_mode")
   (value("bit16", archon_sample_mode::bit16),
    value("bit32", archon_sample_mode::bit32));

 registration::class_<archon_buffer_info>("foxtrot::devices::archon_buffer_info")
   .constructor()(policy::ctor::as_object)
   .property("samppode", &archon_buffer_info::sampmode)
   .property("complete", &archon_buffer_info::complete)
   .property("bufmode", &archon_buffer_info::bufmode)
   .property("offsetaddr", &archon_buffer_info::offsetaddr)
   .property("frame_number", &archon_buffer_info::frame_number)
   .property("width", &archon_buffer_info::width)
   .property("height", &archon_buffer_info::height)
   .property("pixel_progress", &archon_buffer_info::pixel_progress)
   .property("line_progress", &archon_buffer_info::line_progress)
   .property("raw_blocks", &archon_buffer_info::raw_blocks)
   .property("raw_lines", &archon_buffer_info::raw_lines)
   .property("raw_offset", &archon_buffer_info::raw_offset)
   .property("timestamp", &archon_buffer_info::timestamp)
   .property("REtimestamp", &archon_buffer_info::REtimestamp)
   .property("FEtimestamp", &archon_buffer_info::FEtimestamp)
   .property("REAtimestamp", &archon_buffer_info::REAtimestamp)
   .property("FEAtimestamp", &archon_buffer_info::FEAtimestamp)
   .property("REBtimestamp", &archon_buffer_info::REBtimestamp)
   .property("FEBtimestamp", &archon_buffer_info::FEBtimestamp);

 registration::class_<archon_frame_info>("foxtrot::devices::archon_frame_info")
     .constructor()(policy::ctor::as_object)
   .property("current_Time", &archon_frame_info::current_time)
   .property("rbuf", &archon_frame_info::rbuf)
   .property("wbuf", &archon_frame_info::wbuf)
   .property("buffer_infos", &archon_frame_info::buffer_infos);

 registration::class_<archon_module_info>("foxtrot::devices::archon_module_info")
   .constructor()(policy::ctor::as_object)
   .property("position", &archon_module_info::position)
   .property("type", &archon_module_info::type)
   .property("revision", &archon_module_info::revision)
   .property("version", &archon_module_info::version)
   .property("module_id", &archon_module_info::module_id);


 registration::class_<archon_system_info>("foxtrot::devices::archon_system_info")
   .constructor()(policy::ctor::as_object)
   .property("backplane_type", &archon_system_info::backplane_type)
   .property("backplane_rev", &archon_system_info::backplane_rev)
   .property("backplane_version", &archon_system_info::backplane_version)
   .property("backplane_id", &archon_system_info::backplane_id)
   .property("power_id", &archon_system_info::power_id)
   .property("modules", &archon_system_info::modules);


 registration::enumeration<archon_power_status>("foxtrot::devices::archon_power_status")
   (value("unknown", archon_power_status::unknown),
    value("not_configured", archon_power_status::not_configured),
    value("off", archon_power_status::off),
    value("intermediate", archon_power_status::intermediate),
    value("on", archon_power_status::on),
    value("standby", archon_power_status::standby));

 registration::class_<archon_module_status>("foxtrot::devices::archon_module_status")
   .constructor()(policy::ctor::as_object)
   .property("temp", &archon_module_status::temp)
   .property("dinput_status", &archon_module_status::dinput_status)
   .property("HC_Vs", &archon_module_status::HC_Vs)
   .property("HC_Is", &archon_module_status::HC_Is)
   .property("LC_Vs", &archon_module_status::LC_Vs)
   .property("LC_Is", &archon_module_status::LC_Is);


 registration::class_<archon_status>("foxtrot::devices::archon_status")
   .constructor()(policy::ctor::as_object)
   .property("valid", &archon_status::valid)
   .property("count", &archon_status::count)
   .property("nlogs", &archon_status::nlogs)
   .property("powerstatus", &archon_status::powerstatus)
   .property("powergood", &archon_status::powergood)
   .property("overheat", &archon_status::overheat)
   .property("backplane_temp", &archon_status::backplane_temp)
   .property("PSU_map", &archon_status::PSU_map);

 
 
 registration::class_<archon>("foxtrot::devices::archon")
   .method("getStatus", &archon::getStatus)
   .method("getSystem", &archon::getSystem)
   .method("getFrame", &archon::getFrame)
 .method("clear_config",&archon::clear_config)
   .property_readonly("status", &archon::status)
   .property_readonly("frameinfo", &archon::frameinfo)
   .property_readonly("system", &archon::system)
 .property_readonly("fetch_log",&archon::fetch_log)
   .property_readonly("fetch_all_logs", &archon::fetch_all_logs)

   .method("readConfigLine", &archon::readConfigLine)
   (parameter_names("num", "override_existing"))
   .method("readKeyValue", select_overload<std::string(const std::string&), archon>(&archon::readKeyValue))
   (parameter_names("key"))
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
 (parameter_names("buf"))
 .method("fetch_buffer",&archon::fetch_buffer)
 (parameter_names("buf"), metadata("streamdata",true))
 .method("fetch_raw_buffer",&archon::fetch_raw_buffer)
 (parameter_names("buf"), metadata("streamdata",true))
 (parameter_names("buf"))
 .property_readonly("getreset_start",&archon::getreset_start)
 .property_readonly("getreset_end",&archon::getreset_end)
 .property_readonly("getsignal_start",&archon::getsignal_start)
 .property_readonly("getsignal_end",&archon::getsignal_end)
 .method("setCDSTiming",&archon::setCDSTiming)
 (parameter_names("reset_start","reset_end","signal_start","signal_end"))
 .method("settapline", &archon::settapline)
 (parameter_names("n","tapline"))
 .property("trigoutinvert", &archon::gettrigoutinvert, &archon::settrigoutinvert)
 (parameter_names("invert"))
 .property("trigoutpower", &archon::gettrigoutpower, &archon::settrigoutpower)
 .property("rawenable", &archon::getrawenable, &archon::setrawenable)
 (parameter_names("onoff"))
 .property("trigoutlevel", &archon::gettrigoutlevel, &archon::settrigoutlevel)
 (parameter_names("onoff"))
 .property("trigoutforce", &archon::gettrigoutforce, &archon::settrigoutforce)
 (parameter_names("onoff"))
 .property("rawchannel",&archon::getrawchannel,&archon::setrawchannel)
 (parameter_names("ch"))
 .property("rawstartline", &archon::getrawstartline, &archon::setrawstartline)
 (parameter_names("line"))
 .property("rawendline", &archon::getrawendline, &archon::setrawendline)
 (parameter_names("line"))
 .property("rawstartpixel", &archon::getrawstartpixel, &archon::setrawstartpixel)
 (parameter_names("pix"))
 .property("rawsamples", &archon::getrawsamples, &archon::setrawsamples)
 (parameter_names("n"))
 
 .method("setframemode", &archon::setframemode)
 (parameter_names("mode"))
 .property_readonly("getframemode",&archon::getframemode)
 .property_readonly("get_timing_lines",&archon::get_timing_lines)
 .property_readonly("get_states",&archon::get_states)
 .property_readonly("get_constants",&archon::get_constants)
 .property_readonly("get_power",&archon::get_power)
 .property_readonly("get_parameters",&archon::get_parameters)
 .method("settap", &archon::settap)
   (parameter_names("AD","LR","gain","offset"))
   .method("set_tap_pixels", &archon::set_tap_pixels)
   (parameter_names("npixels"))
   .property_readonly("get_tap_pixels",&archon::get_tap_pixels)
   .method("set_tap_lines",&archon::set_tap_lines)
   (parameter_names("lines"))
   .property_readonly("get_tap_linues",&archon::get_tap_lines)

  .method("load_timing", &archon::load_timing)

 ;

 registration::class_<std::map<std::string, std::string>>("std::map<std::string, std::string>")
   .constructor()(policy::ctor::as_object);
    
 foxtrot::register_tuple<std::pair<double, double>>();

 registration::class_<foxtrot::devices::archon_legacy>("foxtrot::devices::archon_legacy")
   .method("update_state",&archon_legacy::update_state)
   .method("isbuffercomplete",&archon_legacy::isbuffercomplete)
    .method("get_frameno", &archon_legacy::get_frameno)
   (parameter_names("buf"))
   .method("get_width", &archon_legacy::get_width)
   (parameter_names("buf"))
   .method("get_height",&archon_legacy::get_height)
   (parameter_names("buf"))
   .method("get_mode",&archon_legacy::get_mode)
   (parameter_names("buf"))
   .method("get_32bit",&archon_legacy::get_32bit)
   .method("get_pixels",&archon_legacy::get_pixels)
   (parameter_names("buf"))
   .method("get_rawlines", &archon_legacy::get_rawlines)
   (parameter_names("buf"))
   .method("get_rawblocks", &archon_legacy::get_rawblocks)
   (parameter_names("buf"));
}

