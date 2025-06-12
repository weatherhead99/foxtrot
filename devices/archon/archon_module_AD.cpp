#include "archon_module_AD.h"
#include "archon_module_mapper.hh"

devices::ArchonAD::ArchonAD(std::weak_ptr<archon>& arch, const archon_module_info& modinf)
: ArchonModule(arch, modinf)
{

}

double devices::ArchonAD::getClamp(int channel)
{

  checkChannelNum(channel);
  _oss.str("");
  _oss << "CLAMP" << channel;

  auto clampstr = readConfigKey(_oss.str());
  return std::stod(clampstr);

}

void foxtrot::devices::ArchonAD::setPreampGain(bool hgain)
{
    _oss.str("");
    _oss << "PREAMPGAIN";
    
    writeConfigKey(_oss.str(), std::to_string(hgain));
}


bool devices::ArchonAD::getPreampGain()
{
  auto str = readConfigKey("PREAMPGAIN");
  return std::stoi(str);
  
}

const string devices::ArchonAD::getTypeName() const
{
  return "AD";

}

void devices::ArchonAD::setClamp(int channel, double val)
{
  checkChannelNum(channel);
  _oss.str("");
  _oss << "CLAMP" << channel;
  
  if(val < -2.5 || val > 2.5)
  {
    throw std::out_of_range("invalid clamp voltage setting!");
  }
  
  writeConfigKey(_oss.str(), std::to_string(val));
}

devices::ArchonADM::ArchonADM(std::weak_ptr<archon> &arch, const archon_module_info& modinf)
  : ArchonModule(arch, modinf) {}

const string devices::ArchonADM::getTypeName() const { return "ADM";}

RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::ArchonAD;
 using foxtrot::devices::ArchonADM;
 
 registration::class_<ArchonAD>("foxtrot::devices::ArchonAD")
 .method("setClamp",&ArchonAD::setClamp)
 (parameter_names("channel","val"))
 .method("getClamp",&ArchonAD::getClamp)
 (parameter_names("channel"))
 .method("setPreampGain",&ArchonAD::setPreampGain)
 (parameter_names("hgain"))
 .property_readonly("getPreampGain",&ArchonAD::getPreampGain)
 ;

 registration::class_<ArchonADM>("foxtrot::devices::ArchonADM")
   ;
 
     
    
}
