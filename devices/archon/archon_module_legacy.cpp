#include "archon_module_legacy.h"
#include "archon.h"
#include <foxtrot/DeviceError.h>
#include <rttr/registration>

using foxtrot::devices::ArchonModuleLegacy;
using foxtrot::devices::archon;

std::string ArchonModuleLegacy::getID() const
{
  std::ostringstream oss;
  oss << std::hex << std::uppercase << std::setfill('0') << std::setw(16)  << _info.module_id;
  return oss.str();
}

const std::array<char, 3>& ArchonModuleLegacy::getVersion() const
{  
    return _version;
}

unsigned short ArchonModuleLegacy::getRev() const
{
    return _info.revision;
}

double ArchonModuleLegacy::getTemp()
{

  std::ostringstream oss;
  oss << "MOD" << (_info.position) << "/" << "TEMP";
  
  auto statmap = getArchon().getStatus();
  auto tempstr = statmap.at(oss.str());
  
  return std::stod(tempstr);
  
}


archon& ArchonModuleLegacy::getArchon()
{
  if(auto arch = _arch.lock())
    return *(arch.get());
  else
    throw foxtrot::DeviceError("parent archon has been deleted, cannot acquire pointer");
}


short unsigned int ArchonModuleLegacy::getmodpos()
{
  return _info.position;

}


RTTR_REGISTRATION {
  using namespace rttr;
 registration::class_<ArchonModuleLegacy>("foxtrot::devices::ArchonModuleLegacy")
   .property_readonly("getID",&ArchonModuleLegacy::getID)
 .property_readonly("getRev",&ArchonModuleLegacy::getRev)
 .property_readonly("getVersion",&ArchonModuleLegacy::getVersion)

   .property_readonly("ID", &ArchonModuleLegacy::getID)
   .property_readonly("Rev", &ArchonModuleLegacy::getRev)
   .property_readonly("Version", &ArchonModuleLegacy::getVersion)
   .property_readonly("modpos", &ArchonModuleLegacy::getmodpos)
   .property_readonly("Temp", &ArchonModuleLegacy::getTemp);


}
