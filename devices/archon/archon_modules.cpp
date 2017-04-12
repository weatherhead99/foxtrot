#include "archon_modules.h"
#include <sstream>

#include "archon_module_heaterx.h"
#include <iomanip>

#include <iostream>

#include <iostream>
#include <rttr/registration>

using namespace foxtrot::devices;


using foxtrot::devices::ArchonModule;
using foxtrot::devices::archon_module_types;
using foxtrot::devices::archon_hex_stream_configure;
  
ArchonModule::ArchonModule(archon& arch, short unsigned modpos)
: _arch(arch), _modpos(modpos), foxtrot::Device(nullptr)
{
  
  
    auto statmap = _arch.getSystem();
    
    std::ostringstream oss;
    
    
    oss << "MOD" << _modpos << "_TYPE";
//     std::cout << "reqstr: " << oss.str() << std::endl;
    
    _modtype = static_cast<archon_module_types>(std::stoi(statmap.at(oss.str())));
//     std::cout << "modtype recorded" << std::endl;
    oss.str("");
    oss.clear();
    
    oss << "MOD" << _modpos << "_REV";
//     std::cout << "reqstr: " << oss.str() << std::endl;
    
    _rev = static_cast<short unsigned>(std::stoi(statmap.at(oss.str())));
//     std::cout << "rev:" << _rev << std::endl;
    oss.str("");
    oss.clear();
    
    oss << "MOD" << _modpos << "_VERSION";
    std::istringstream iss(statmap.at(oss.str()));
    oss.str("");
    oss.clear();
    
    
    for(auto& n : _version)
    {
      std::string verspart;
      std::getline(iss,verspart,'.');
      n = std::stoi(verspart);
    };
    
    
    oss << "MOD" << _modpos << "_ID" ;
    _id = statmap.at(oss.str());
    
    std::cout << "id: " << _id << std::endl;
    
}




const std::string& ArchonModule::getID() const
{
    return _id;
}

const std::array<char, 3>& ArchonModule::getVersion() const
{
    return _version;
}

unsigned short ArchonModule::getRev() const
{
    return _rev;
}

double ArchonModule::getTemp()
{
  
  
  std::ostringstream oss;
  oss << "MOD" << (_modpos+1) << "/" << "TEMP";
  
  auto statmap = _arch.getStatus();
  auto tempstr = statmap.at(oss.str());
  
  return std::stod(tempstr);
  
}


const archon& ArchonModule::getArchon()
{
  return _arch;

}



void ArchonModule::update_variables()
{
  throw std::logic_error("called unimplemented function base update_variables");
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

string ArchonModule::readConfigKey(const string& subkey)
{
  std::ostringstream oss;
  oss << "MOD" << (_modpos+1) << "/" << subkey;
  
  return _arch.readKeyValue(oss.str());
  
}

void ArchonModule::writeConfigKey(const string& key, const string& val)
{
  std::ostringstream oss;
  oss << "MOD" <<  (_modpos+1) << "/" << key ;
  
  _arch.writeKeyValue(oss.str(),val);

}


void ArchonModule::apply()
{
  std::ostringstream oss;
  
  oss << "APPLYMOD" ;
//   archon_hex_stream_configure(oss);
  oss << std::setw(2) << std::setfill('0') << std::hex << std::uppercase  << (_modpos );

  //TODO: should find a way round this not needing to be a friend!
  _arch.cmd(oss.str());
  

}



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::ArchonModule;
 
 //TODO get version
 
 registration::class_<ArchonModule>("foxtrot::devices::ArchonModule")
 .property_readonly("getID",&ArchonModule::getID)
 .property_readonly("getRev",&ArchonModule::getRev)
 .property_readonly("getVersion",&ArchonModule::getVersion)
 .method("writeConfigKey",&ArchonModule::writeConfigKey)
 (
     parameter_names("key","val")
     
 )
 .method("readConfigKey",&ArchonModule::readConfigKey)
 (
     parameter_names("key")
     )
 .method("apply",&ArchonModule::apply)
 ;
    
    
}


