#include "archon_modules.h"
#include <sstream>

#include "archon_module_heaterx.h"

using foxtrot::devices::ArchonModule;

foxtrot::devices::ArchonModule::ArchonModule(foxtrot::devices::archon& arch, short unsigned modpos)
: _arch(arch), _modpos(modpos), foxtrot::Device(nullptr)
{
    auto statmap = _arch.getSystem();
    
    std::ostringstream oss;
    
    oss << "MOD" << _modpos << "_TYPE";
    _modtype = static_cast<archon_module_types>(std::stoi(statmap[oss.str()]));
    oss.clear();    
    
    oss << "MOD" << _modpos << "_REV";
    _rev = static_cast<short unsigned>(std::stoi(statmap[oss.str()]));
    oss.clear();
    
    oss << "MOD" << _modpos << "_VERSION";
    std::istringstream iss(statmap[oss.str()]);
    oss.clear();
    
    for(auto& n : _version)
    {
      std::string verspart;
      std::getline(iss,verspart,'.');
      n = std::stoi(verspart);
    };
    
    iss.clear();
    
    oss << "MOD" << _modpos << "_ID" ;
    iss.str(statmap[oss.str()]);
    
    for(auto& ch : _id)
    {
      ch = iss.get(); 
    };
    
}



const std::array<char, 16>& foxtrot::devices::ArchonModule::getID() const
{
    return _id;
}

const std::array<char, 3>& foxtrot::devices::ArchonModule::getVersion() const
{
    return _version;
}

unsigned short foxtrot::devices::ArchonModule::getRev() const
{
    return _rev;
}


void foxtrot::devices::ArchonModule::update_variables()
{
  throw std::logic_error("called unimplemented function base update_variables");
}


string devices::get_module_variable_string(int modpos, const string& name, const ssmap& map, char delim)
{
      std::ostringstream oss;
      oss << "MOD" << modpos << delim << name;
      
      return map.at(oss.str());
}

