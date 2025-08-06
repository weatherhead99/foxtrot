#include "archon_modules.h"
#include "archon_module_mapper.hh"
#include <boost/token_functions.hpp>
#include <sstream>
#include <foxtrot/ft_union_helper.hh>

#include <iomanip>

#include <iostream>

#include <iostream>
#include <map>
#include <rttr/registration>
#include <foxtrot/DeviceError.h>
#include <stdexcept>

using namespace foxtrot::devices;


using foxtrot::devices::ArchonModule;
using foxtrot::devices::archon_module_types;
using foxtrot::devices::archon_hex_stream_configure;



ArchonModule::ArchonModule(std::weak_ptr<archon>& arch, const archon_module_info& modinfo)
  : foxtrot::Device(nullptr),  _info(modinfo), _arch(arch), lg("ArchonModule")
{
    
    std::istringstream iss(_info.version);
    for(auto& n : _version)
    {
      std::string verspart;
      std::getline(iss,verspart,'.');
      n = std::stoi(verspart);
    };

}

void ArchonModule::status(archon_module_status& out, const ssmap& statusmap) const
{
   auto findstr = std::format("MOD{}/TEMP", _info.position);
   out.temp = std::stod(statusmap.at(findstr));
}


archon_module_status ArchonModule::status(const ssmap& statusmap) const
{
 
  archon_module_status out;
  status(out, statusmap);
  return out;
}

archon_module_status ArchonModule::status() const
{
  if(auto ptr = _arch.lock())
    {
      auto smap = ptr->getStatus();
      return this->status(smap);
    }
  else
    throw std::logic_error("couldn't lock archon pointer");
}

const archon_module_info& ArchonModule::info() const
{
  return _info;
}




string foxtrot::devices::get_module_variable_string(int modpos, const string& name, const ssmap& map, char delim)
{
      std::ostringstream oss;
      //WARNING: +1 to get from zero-indexed to 1 indexed
      
      oss << "MOD" << (modpos +1)<< delim << name;
//       std::cout << "getstr:" << oss.str() << std::endl;
      string val;
      
      try{
      val = map.at(oss.str());
      }
      catch(std::out_of_range& err)
      {
	std::cout << "caught out of range for key: "<< oss.str() << std::endl;
	for(auto& item: map)
	{
	 std::cout << "*" << item.first << "* \t *" << item.second << "*" << std::endl; 
	}
	throw err;
      }
      
      return val;
}


std::optional<string> ArchonModule::readConfigKeyOpt(const string& subkey) const
{
  std::optional<string> out = std::nullopt;

  auto cmdstr = std::format("MOD{}/{}", _info.position, subkey);
  if(auto ptr = _arch.lock())
    {
      //TODO: more efficient way of doing this
      auto* val = ptr->readKeyValueOpt(cmdstr);
      if(val == nullptr)
	return std::nullopt;
      return *val;
    }
  throw std::logic_error("couldn't lock archon pointer from submodule!");
  
  
  return out;

}

string ArchonModule::readConfigKey(const string& subkey) const
{
  auto out = readConfigKeyOpt(subkey);
  if(not out.has_value())
    {
      auto outstr = std::format("subkey: {} not found in archon config", subkey);
      throw std::out_of_range(outstr);
      }

  return *out;
}

void ArchonModule::writeConfigKey(const string& key, const string& val)
{
  std::ostringstream oss;
  auto cmdstr = std::format("MOD{}/{}", _info.position, key);
  if(auto ptr = _arch.lock())
    ptr->writeKeyValue(cmdstr,val);
  else
    throw std::logic_error("couldn't lock archon pointer from submodule");
}

std::vector<ArchonModuleProp> ArchonModule::props(const ssmap& statusmap) const
{
  std::vector<ArchonModuleProp> out;
  return out;
}

std::vector<ArchonModuleProp> ArchonModule::props()
{
  if(auto ptr = _arch.lock())
    {
      auto smap = ptr->getStatus();
      return this->props(smap);
    }
  else {
    throw std::logic_error("couldn't lock archon pointer");
      }
}


void ArchonModule::apply()
{

  //NOTE: it looks like unlike the values in the "status" return,
  //the position here is numbered from 0... because OF COURSE it is

  //no idea how this was working for 9 years without knowing that....
  std::ostringstream oss;
  
  oss << "APPLYMOD" ;
//   archon_hex_stream_configure(oss);
  oss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase  << (_info.position-1);

  //TODO: should find a way round this not needing to be a friend!
  if(auto ptr = _arch.lock())
    ptr->cmd(oss.str());

  else
    throw std::logic_error("archon device object destroyed, can't access from submodule");
  
}



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::ArchonModule;
 
 //TODO get version
 
 registration::class_<ArchonModule>("foxtrot::devices::ArchonModule")

   .method("apply",&ArchonModule::apply)
   .method("status",
	   rttr::select_overload<archon_module_status() const>(&ArchonModule::status))
   .method("props",
   	     rttr::select_overload<std::vector<ArchonModuleProp>()>(&ArchonModule::props))
   
 ;

 
}


