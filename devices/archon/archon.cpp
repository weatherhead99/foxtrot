#include <algorithm>
#include <stdexcept>
#include <system_error>
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
#include <foxtrot/ft_timestamp_helper.hh>

#include <foxtrot/protocols/ProtocolUtilities.h>
#include <foxtrot/protocols/CommunicationProtocol.h>

#include <foxtrot/ft_optional_helper.hh>

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
using foxtrot::protocols::simpleTCPBase;

ArchonStreamHelper::ArchonStreamHelper(archon& dev) : _dev(dev) {}


struct foxtrot::devices::detail::archonimpl
{
  std::unordered_map<std::string, std::pair<std::string, int>> parammap;
  std::unordered_map<std::string, std::pair<std::string, int>> constvals;

  std::vector<std::string> configindex;
  bool mapvalid;
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
    out->read_parse_existing_config(true);
  out->setup_modules();

  return out;
};


foxtrot::devices::archon::archon(std::shared_ptr< foxtrot::protocols::simpleTCPBase > proto)
  : CmdDevice(std::static_pointer_cast<foxtrot::CommunicationProtocol>(proto)), _specproto(proto),
    _lg("archon"), _order(0)
{
  impl = std::make_unique<detail::archonimpl>();

  proto->get_timeout();
  _lg.strm(sl::info) << "got to here!!!!!!";
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


void devices::archon::load_config(const std::string& cfg)
{
  clear_config();
  std::istringstream iss(cfg);
  std::string line;
  int i=0;
  while(std::getline(iss, line))
    {
      //NOTE: -1 here for new line
      writeConfigLine(line, -1);
    };

  read_parse_existing_config();
};

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
    {
      auto inf = mod->info();
      _lg.strm(sl::debug) << "module position at: " << inf.position << " of type: " << mod->getTypeName();
      try
	{
	  out.module_statuses.push_back(mod->status(statmap));
	}
      catch(std::out_of_range& err)
	{
	  _lg.strm(sl::error) << "got an error, returning what we have anyway";
	}
    }

  return out;
}

unsigned long long archon::timer()
{
      auto str = cmd("TIMER");
    auto eqpos = std::find(str.begin(), str.end(),'=');
    std::string count(eqpos+1,str.end());
    
    _lg.strm(sl::debug) << "timerstr: '" << count << "'";
    
    return std::stoull(count, nullptr, 16);
}

foxtrot::devices::HRTimePoint archon::current_time()
{
  auto tmr = timer();
  return archon_time_to_real_time(tmr);
}

foxtrot::devices::archon_frame_info archon::frameinfo()
{
  archon_frame_info out;
  auto framemap = getFrame();
  out.rbuf = std::stoul(framemap.at("RBUF"));
  out.wbuf = std::stoul(framemap.at("WBUF"));
  out.current_time = current_time();
  
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
  //  _configlinemap.clear();
  _configmap.clear();
  impl->configindex.clear();
  impl->mapvalid = false;
  //_statenames.clear();
  //_parammap.clear();
  //_constantmap.clear();
  
  _ADtaplinemap.clear();
  //  _taplines = 0;
  //  _states=  0;  
  //setup lines, timing lines etc...
  //  set_timing_lines(0);
  //set_states(0);
  //set_parameters(0); 
  //set_constants(0);
}


int devices::archon::writeConfigLine(const string& line,int num)
{
  //check if possible
  if(impl->configindex.size() >= ARCHON_MAX_CONFIG_LINES)
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
    num = impl->configindex.size();
  }
  else if(static_cast<unsigned>(num) > impl->configindex.size())
  {
    throw std::logic_error("trying to overwrite a config line " + std::to_string(num) + " that doesn't exist yet");
  };
  std::ostringstream oss;
  oss << "WCONFIG" << std::setw(4) << std::setfill('0') << std::uppercase<<  std::hex << num << line;
  cmd(oss.str());

  return num;

}


std::string devices::archon::readConfigLine(unsigned num, bool override_existing)
{
  if( num >= impl->configindex.size() && !override_existing)
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

std::optional<int> devices::archon::find_config_line_from_key(const std::string& key)
{
  auto linumit = std::find(impl->configindex.begin(),
			   impl->configindex.end(), key);
  if(linumit == impl->configindex.end())
    {
      return std::nullopt;
    }

  return std::distance(impl->configindex.begin(), linumit);
  //NOTE: do we need an off by one here?
}


const std::string& devices::archon::readKeyValue(const string& key)
{
  //NOTE: bounds checked by the .at call
  return _configmap.at(key);
}

const std::string* const devices::archon::readKeyValueOpt(const string& key)
{
  if(!_configmap.contains(key))
    return nullptr;
  return &_configmap.at(key);
}


void devices::archon::writeKeyValue(const string& key, const string& val)
{
  auto linestr = std::format("{}={}",key,val);
  auto linum = find_config_line_from_key(key);
  if(not linum.has_value())
    {
    //this is a new key 
      writeConfigLine(linestr);
      impl->configindex.push_back(key);
      _configmap.insert({key, val});
    }
  else
    {
      writeConfigLine(linestr,*linum);
      _configmap.at(key) = val;
    }

  impl->mapvalid = false;
  //NOTE: is this exception safe?
  //update the mappings etc
}


// void devices::archon::set_timing_lines(int n)
// {
//   writeKeyValue("LINES",std::to_string(n));

// }

// int devices::archon::get_timing_lines()
// {
//   return std::stoi(readKeyValue("LINES"));

// }

// int devices::archon::get_states()
// {
//   return std::stoi(readKeyValue("STATES"));

// }

// void devices::archon::set_states(int n)
// {
//   writeKeyValue("STATES",std::to_string(n));
// }

// int devices::archon::get_constants()
// {
//   return std::stoi(readKeyValue("CONSTANTS"));
// }

// void devices::archon::set_constants(int n)
// {
//   writeKeyValue("CONSTANTS",std::to_string(n));
// }

// int devices::archon::get_parameters()
// {
//   return std::stoi(readKeyValue("PARAMETERS"));
// }

// void devices::archon::set_parameters(int n)
// {
//   writeKeyValue("PARAMETERS",std::to_string(n));
// }

void foxtrot::devices::archon::set_power(bool onoff)
{
    if(onoff)
      {
	//note, can take a long time, so setup timeout appropriately (20 seconds)
	_lg.strm(sl::trace) << "getting existing timeout";
	auto tm_bef = _specproto->get_timeout();

	_lg.strm(sl::trace) << "setting a long timeout...";
        _specproto->set_timeout(std::chrono::milliseconds(20000));
	try {
	  _lg.strm(sl::trace) << "running comand...";
	  cmd("POWERON");
	}
	catch(foxtrot::ProtocolError& err)
	  {
	    _lg.strm(sl::error) << "resetting timeout...";
	    _specproto->set_timeout(tm_bef);
	    throw err;
	  }
	_lg.strm(sl::trace) << "restore original timeout";
	_specproto->set_timeout(tm_bef);
      }
    else
      cmd("POWEROFF");   
}

foxtrot::devices::archon_power_status devices::archon::get_power()
{
  auto power = std::stoi(getStatus().at("POWER"));
  return static_cast<archon_power_status>(power);
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
    writeKeyValue("LINES",std::to_string(i+1));
    
}


void foxtrot::devices::archon::lockbuffer(int buf)
{
    if(buf < 1 || buf > 3)
      throw std::out_of_range("invalid buffer, must be 1 :3 ");

    cmd("LOCK" + std::to_string(buf));
}

const std::unordered_map<std::string, std::string>& devices::archon::config() const
{
  return _configmap;
}

std::vector<std::pair<std::string, std::string>> devices::archon::ordered_config() const
{
  std::vector<std::pair<std::string,std::string>> out;

  out.reserve(impl->configindex.size());
  for(std::size_t i=0; i < impl->configindex.size(); i++)
    {
      auto& k = impl->configindex[i];
      auto& v = _configmap.at(k);
      out.push_back({k, v});
    }
  return out;
}

std::pair<std::string_view, std::string_view> spliteq(const std::string& in)
{
  auto eqpos = std::find(in.begin(), in.end(),'=');
  auto k = std::string_view(in.begin(), eqpos);
  auto v = std::string_view(eqpos+1, in.end());
  return {k, v};
}


std::unordered_map<std::string, int> devices::archon::params()
{
  unsigned n_params = std::stoul(readKeyValue("PARAMETERS"));
  std::unordered_map<string, int> out;
  if(impl->mapvalid == false)
    {
      impl->parammap.clear();
      for(unsigned i=0; i < n_params; i++)
	{
	  auto key = std::format("PARAMETER{}", i);
	  auto val = readKeyValue(key);
	  _lg.strm(sl::debug) << "value string is: " << val;

	  auto [paramk, paramv] = spliteq(val);
	  
	  //	  auto eqpos = std::find(val.begin(), val.end(), '=');
	  //auto paramk = std::string(val.begin(), eqpos);
	  //auto paramv = std::string(eqpos + 1, val.end());
	  int param = 0;
	  auto res = std::from_chars(paramv.data(), paramv.data()+ paramv.size(), param);
	  if(res.ec == std::errc::invalid_argument)
	    {
	      _lg.strm(sl::error) << "offending paramk: {" << paramk << "}";
	      _lg.strm(sl::error) << "offending paramv: {" << paramv << "}"; 
	      throw std::logic_error("bad value when converting parameter from string");
	    }
	  out.emplace(paramk, param);
	  impl->parammap.emplace(paramk, std::make_pair(paramv, i));
	}
      impl->mapvalid = true;
    }
  else
    {
      for(auto [k,v] : impl->parammap)
	{
	  out.emplace(k, std::stoi(v.first));
	}
    }
    return out;
}

void foxtrot::devices::archon::set_param(const std::string& name, int val, bool apply_immediate, bool allow_new)
{
  //reload parameter map
  if(impl->mapvalid == false)
    //side effect of this is to force update
    auto parammap_temp = params();

  //if not already this will throw
  int paramnum;
  auto writestr = std::format("{}={}", name, val);
  
  try {
    auto [paramval, paramidx] = impl->parammap.at(name);
    paramnum = paramidx;
  }
  catch(std::out_of_range& err)
    {
      if(not allow_new)
	{
	  _lg.strm(sl::error) << "parameter: " << name << "not in registry";
	  throw foxtrot::DeviceError("tried to set unknown parameter");
	}
      //case for new parameter with allow_new
      auto next_param_num = impl->parammap.size();
      writeKeyValue(std::format("PARAMETER{}",next_param_num),writestr);
      writeKeyValue("PARAMETERS",impl->parammap.size() +1);
      impl->mapvalid = false;
      return;
    }

  //this case is a known parameter, changing value
  writeKeyValue(std::format("PARAMETER{}",paramnum), writestr);
  //keyword "PARAMETERS" doesn't need to change

  if(apply_immediate)
    cmd(std::format("LOADPARAM {}", name));

}

using foxtrot::devices::ArchonModuleProp;

std::map<int, std::vector<ArchonModuleProp>>
foxtrot::devices::archon::moduleprops()
{
  std::map<int, std::vector<ArchonModuleProp>> out;
  if(get_power() == archon_power_status::not_configured)
    {
      _lg.strm(sl::warning) << "no configuration loaded, returning empty module props";
      return out;
    }
  
  auto statmap = this->getStatus();

  
  for(auto& [ind, mptr] : _modules)
    {
      auto modinf = mptr->info();
      _lg.strm(sl::debug) << "processing props for module at position: " << modinf.position << "of type: " << mptr->getTypeName();
      
      auto thisprop = mptr->props(statmap);
      out[ind] = std::move(thisprop);
    }

  return out;
}


void foxtrot::devices::archon::unlockbuffers()
{
    cmd("LOCK0");
}

void foxtrot::devices::archon::read_parse_existing_config(bool allow_empty)
{
    //NOTE: need to write at least one config line because reading empty config results in infinite loop
  auto stat = status();
  if(stat.powerstatus == foxtrot::devices::archon_power_status::not_configured)
    {
      if(not allow_empty)
	throw foxtrot::DeviceError("no configuration loaded in archon, cannot parse!");
      
      _configmap.clear();
      impl->configindex.clear();
      return;
    }
  _configmap.clear();
  impl->configindex.clear();
  int i;
  impl->configindex.reserve(ARCHON_MAX_CONFIG_LINES);
  for(i =0 ; i < ARCHON_MAX_CONFIG_LINES; i++)
    {
        auto confline = readConfigLine(i,true);
        if(confline.size() == 0)
            break;
        auto eqpos = std::find(confline.begin(), confline.end(),'=');
        if(eqpos == confline.end())
	  throw DeviceError("malformed config line returned from archon");    
        auto key = string(confline.begin(),eqpos);
        auto val = string(eqpos+1,confline.end());
	_configmap.insert({key, val});
	impl->configindex.push_back(key);
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

std::pair<std::string, std::string> splitconfline(const std::string& confline)
{
  auto eqpos = std::find(confline.begin(), confline.end(), '=');
  auto quotepos = std::find(eqpos, confline.end(), '\"');
  auto key = std::string(confline.begin(), eqpos);
  auto val = std::string(eqpos+1, quotepos);
  return {key, val};
}

// double devices::archon::getConstant(const string& name)
// {
//    int confnum;
//     try{
//         confnum = _constantmap.at(name);
//     }
//     catch(std::out_of_range& err)
//     {
//         throw DeviceError("parameter not known to this archon");
//     }
//     auto paramline = readKeyValue(std::format("CONSTANT{}",confnum));
//     auto [k, v] = splitconfline(paramline);
//     return std::stod(v);

// }

// void devices::archon::setConstant(const string& name, double val)
// {
//     int confnum;
//     try{
//         confnum = _constantmap.at(name);
//     }
//     catch(std::out_of_range& err)
//     {   
//         confnum = get_constants();
//         _constantmap[name] = confnum;
//         set_constants(confnum +1);
//     }
    
//     std::ostringstream osskey;
//     osskey << "CONSTANT" << confnum;
    
//     std::ostringstream ossval;
//     ossval << name << '=' << val ;
    
//   writeKeyValue(osskey.str(),ossval.str());
// }


// unsigned int devices::archon::getParam(const string& name)
// {
//     int confnum;
//     try{
//         confnum = _parammap.at(name);
//     }
//     catch(std::out_of_range& err)
//     {
//         throw DeviceError("parameter not known to this archon");
//     }
    
//     std::ostringstream oss;
//     oss << "PARAMETER" << confnum;
//     auto paramline = readKeyValue(oss.str());
//     auto eqpos = std::find(paramline.begin(),paramline.end(),'=');
//     auto quotepos = std::find(eqpos, paramline.end(),'\"');
//     //TODO: this will fail at runtime
//     return std::stoul(std::string(eqpos+1,quotepos));

// }


// void devices::archon::setParam(const string& name, unsigned int val)
// {
//     int confnum;
//     try{
//         confnum = _parammap.at(name);
//     }
//     catch(std::out_of_range& err)
//     {   
//         confnum = get_parameters();
//         _parammap[name] = confnum;
//         set_parameters(confnum +1);
//     }
    
//     std::ostringstream osskey;
//     osskey << "PARAMETER" << confnum;
    
//     std::ostringstream ossval;
//     ossval << name << '=' << val ;
    
//   writeKeyValue(osskey.str(),ossval.str());
// }

// void devices::archon::write_timing_state(string name, const string& state)
// {
//   std::stringstream ss(state);
//   std::string to;
//   auto findstate = std::find(_statenames.begin(),_statenames.end(),name);
//   auto state_idx = std::distance(_statenames.begin(),findstate);
  
//   bool newstate = ( findstate  == _statenames.end() );
  
  
//   while(std::getline(ss,to,'\n'))
//   {
//      auto eqpos = std::find(to.begin(),to.end(),'=');
//      if(eqpos == to.end())
//      {
//        throw std::runtime_error("malformed timing state line: " + to);
//      }
     
//      std::string configkey(to.begin(),eqpos);
//      configkey = "STATE" + std::to_string(state_idx) + "/" + configkey; 
//      std::string val(eqpos + 1, to.end());
     
//      writeKeyValue(configkey, val);
//   }
  
//   //NOTE: do this now in case of exceptions
//   if(newstate)
//   {
//     set_states(_statenames.size());
//    _statenames.insert(_statenames.end(),name);
//   }
   
   

// }

void devices::archon::settapline(int n, const string& tapline)
{
  if(static_cast<unsigned>(n) >= _ADtaplinemap.size() )
  {
    throw DeviceError("invalid TAP line number");
  }
  std::ostringstream oss;
  oss << "TAPLINE" << n;
  writeKeyValue(oss.str(),tapline);
  writeKeyValue("TAPLINES", std::to_string(_ADtaplinemap.size()));
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
	  _ADtaplinemap[AD] = _ADtaplinemap.size();
	  settapline(_ADtaplinemap.size(), taplinestr);
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
    _ADtaplinemap[AD] = _ADtaplinemap.size();
    settapline(_ADtaplinemap.size(), taplinestr);
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
    _arch_tmr = timer();
    _sys_tmr = std::chrono::high_resolution_clock::now();
}


devices::archon_tap_info devices::archon::tapinfo()
{
  archon_tap_info out;
  out.rawstartpixel = std::stoul(readKeyValue("RAWSTARTPIXEL"));
  out.rawstartline = std::stoul(readKeyValue("RAWSTARTLINE"));
  out.rawendline = std::stoul(readKeyValue("RAWENDLINE"));
  out.rawsamples = std::stoul(readKeyValue("RAWSAMPLES"));
  out.rawenable = std::stoul(readKeyValue("RAWENABLE"));
  out.rawchannel = std::stoul(readKeyValue("RAWSEL"));
  out.lines = std::stoul(readKeyValue("LINECOUNT"));
  out.pixels = std::stoul(readKeyValue("PIXELCOUNT"));
  out.framemode = static_cast<archon_buffer_mode>(std::stoul(readKeyValue("FRAMEMODE")));
  out.samplemode = static_cast<archon_sample_mode>(std::stoul(readKeyValue("SAMPLEMODE")));

  out.CDSTiming[0] = std::stoul(readKeyValue("SHP1"));
  out.CDSTiming[1] = std::stoul(readKeyValue("SHP2"));
  out.CDSTiming[2] = std::stoul(readKeyValue("SHD1"));
  out.CDSTiming[3] = std::stoul(readKeyValue("SHD2"));

  return out;
}

void devices::archon::set_tapinfo(const devices::archon_tap_info& tapinfo)
{
  writeKeyValue("RAWSTARTPIXEL", std::to_string(tapinfo.rawstartpixel));
  writeKeyValue("RAWSAMPLES", std::to_string(tapinfo.rawsamples));
  writeKeyValue("RAWSTARTLINE", std::to_string(tapinfo.rawstartline));
  writeKeyValue("RAWENDLINE", std::to_string(tapinfo.rawendline));
  writeKeyValue("PIXELCOUNT", std::to_string(tapinfo.pixels));
  writeKeyValue("LINECOUNT", std::to_string(tapinfo.lines));
  writeKeyValue("RAWSEL", std::to_string(tapinfo.rawchannel));
  writeKeyValue("RAWENABLE", std::to_string(tapinfo.rawenable));
  writeKeyValue("FRAMEMODE", std::to_string(static_cast<int>(tapinfo.framemode)));
  writeKeyValue("SAMPLEMODE", std::to_string(static_cast<int>(tapinfo.samplemode)));

  writeKeyValue("SHP1", std::to_string(tapinfo.CDSTiming[0]));
  writeKeyValue("SHP2", std::to_string(tapinfo.CDSTiming[1]));
  writeKeyValue("SHD1", std::to_string(tapinfo.CDSTiming[2]));
  writeKeyValue("SHD2", std::to_string(tapinfo.CDSTiming[3]));

  cmd("APPLYCDS");
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





RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::archon;;


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
   .property("current_time", &archon_frame_info::current_time)
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
   .property("position", &archon_module_status::position)
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
   .property("PSU_map", &archon_status::PSU_map)
   .property("fanspeed", &archon_status::fanspeed)
   .property("module_statuses", &archon_status::module_statuses);

 using foxtrot::devices::archon_tap_info;
 registration::class_<archon_tap_info>("foxtrot::devices::archon_tap_info")
   .constructor()(policy::ctor::as_object)
   .property("pixels", &archon_tap_info::pixels)
   .property("lines", &archon_tap_info::lines)
   .property("rawchannel", &archon_tap_info::rawchannel)
   .property("rawenable", &archon_tap_info::rawenable)
   .property("rawstartline", &archon_tap_info::rawstartline)
   .property("rawendline", &archon_tap_info::rawendline)
   .property("rawstartpixel", &archon_tap_info::rawstartpixel)
   .property("rawsamples", &archon_tap_info::rawsamples)
   .property("framemode", &archon_tap_info::framemode)
   .property("samplemode", &archon_tap_info::samplemode)
   .property("CDSTiming", &archon_tap_info::CDSTiming);


 foxtrot::register_timestamp<foxtrot::devices::HRTimePoint>();
 
 registration::class_<archon>("foxtrot::devices::archon")
 .method("clear_config",&archon::clear_config)
   .property_readonly("status", &archon::status)
   .property_readonly("frameinfo", &archon::frameinfo)
   .property_readonly("system", &archon::system)
 .property_readonly("fetch_log",&archon::fetch_log)
   .property_readonly("fetch_all_logs", &archon::fetch_all_logs)
   .property_readonly("config", &archon::config)
   .property_readonly("ordered_config", &archon::ordered_config)
   .property_readonly("timer", &archon::timer)
   .method("readConfigLine", &archon::readConfigLine)
   (parameter_names("num", "override_existing"))
   .method("readKeyValue", select_overload<std::string(const std::string&), archon>(&archon::readKeyValue))
   (parameter_names("key"))
 .method("applyall",&archon::applyall)
   .method("load_config", &archon::load_config)
   (parameter_names("cfg"))
   .method("read_parse_existing_config", &archon::read_parse_existing_config)
   (parameter_names("allow_empty"))
 .method("set_power",&archon::set_power)
 .method("load_timing_script", &archon::load_timing_script)
   .method("set_param", &archon::set_param)
   (parameter_names("name", "val", "apply_immediate", "allow_new"))
   //   .method("getParam", select_overload<unsigned(const std::string&), archon>(&archon::getParam))
   //   (parameter_names("name"))
 // .method("setParam",&archon::setParam)
 // (
 //   parameter_names("name","val")
 //   )
 // .method("getConstant",&archon::getConstant)
 // (
 //   parameter_names("name")
 //   )
 // .method("setConstant", &archon::setConstant)
 // (
 //   parameter_names("name","val")
 //   )

 .method("apply_param", &archon::apply_param)
 (
   parameter_names("name")
   )
 .method("apply_all_params", &archon::apply_all_params)
 .method("params", &archon::params)  
 .method("holdTiming", &archon::holdTiming)
 .method("releaseTiming",&archon::releaseTiming)
 .method("resetTiming", &archon::resetTiming)
 .method("lockbuffer",&archon::lockbuffer)
 (
   parameter_names("buf")
   )
 .method("unlockbuffers",&archon::unlockbuffers)
 // .method("write_timing_state",&archon::write_timing_state)
 // (
 //   parameter_names("name", "state")
 //   )
 // (parameter_names("buf"))
 .method("fetch_buffer",&archon::fetch_buffer)
 (parameter_names("buf"), metadata("streamdata",true))
 .method("fetch_raw_buffer",&archon::fetch_raw_buffer)
 (parameter_names("buf"), metadata("streamdata",true))
 (parameter_names("buf"))
 (parameter_names("reset_start","reset_end","signal_start","signal_end"))
   //.method("settapline", &archon::settapline)
   //(parameter_names("n","tapline"))
 .property("trigoutinvert", &archon::gettrigoutinvert, &archon::settrigoutinvert)
 (parameter_names("invert"))
 .property("trigoutpower", &archon::gettrigoutpower, &archon::settrigoutpower)
   .property_readonly("tapinfo", &archon::tapinfo)
   .method("set_tapinfo", &archon::set_tapinfo)
   (parameter_names("tapinfo"))
 .property("trigoutlevel", &archon::gettrigoutlevel, &archon::settrigoutlevel)
 (parameter_names("onoff"))
 .property("trigoutforce", &archon::gettrigoutforce, &archon::settrigoutforce)
 (parameter_names("onoff"))
   //.property_readonly("get_timing_lines",&archon::get_timing_lines)
   // .property_readonly("get_states",&archon::get_states)
   // .property_readonly("get_constants",&archon::get_constants)
 .property_readonly("get_power",&archon::get_power)
   //.property_readonly("get_parameters",&archon::get_parameters)
 .method("settap", &archon::settap)
   (parameter_names("AD","LR","gain","offset"))
  .method("load_timing", &archon::load_timing)
   .property_readonly("moduleprops", &archon::moduleprops)

 ;

 registration::class_<std::map<std::string, std::string>>("std::map<std::string, std::string>")
   .constructor()(policy::ctor::as_object);

 registration::class_<std::unordered_map<std::string, std::string>>("std::unordered_map<std::string, std::string>>")
   .constructor()(policy::ctor::as_object);

 foxtrot::register_tuple<std::pair<double, double>>();
 foxtrot::register_tuple<std::pair<std::string,std::string>>();

 foxtrot::register_optional<std::optional<unsigned>>();
 foxtrot::register_optional<std::optional<unsigned char>>();
 foxtrot::register_optional<std::optional<vector<double>>>();

 foxtrot::register_optional<std::optional<vector<unsigned>>>();

 foxtrot::register_optional<std::optional<int>>();
 foxtrot::register_optional<std::optional<bool>>();
 

}

