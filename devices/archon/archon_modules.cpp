#include "archon_modules.h"
#include <sstream>

foxtrot::devices::ArchonModule::ArchonModule(foxtrot::devices::archon& arch, short unsigned modpos)
: _arch(arch), _modpos(modpos), foxtrot::Device(nullptr)
{
    auto statmap = _arch.getStatus();
    
    std::ostringstream oss;
    
    oss << "MOD" << _modpos << "_TYPE";
    _modtype = static_cast<archon_module_types>(std::stoi(statmap[oss.str()]));
    oss.clear();    
    
    oss << "MOD" << _modpos << "_REV";
    _rev = static_cast<short unsigned>(std::stoi(statmap[oss.str()]));
    oss.clear();
    
    oss << "MOD" << _modpos << "_VERSION";
    std::istringstream iss(statmap[oss.str()]);
    
    
    
    for(int i =0; i < 3; i++)
    {
      std::string verspart;
      std::getline(iss,verspart,'.');
      _version[i] = std::stoi(verspart);
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

 
