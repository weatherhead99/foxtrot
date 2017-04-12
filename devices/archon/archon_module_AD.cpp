#include "archon_module_AD.h"

std::unique_ptr< devices::ArchonModule > devices::ArchonAD::constructModule(devices::archon& arch, int modpos)
{
  std::unique_ptr<foxtrot::devices::ArchonModule> out(new ArchonAD(arch,modpos));
  
  return out;
  

}


devices::ArchonAD::ArchonAD(devices::archon& arch, short unsigned int modpos)
: ArchonModule(arch, modpos)
{

}


void devices::ArchonAD::update_variables()
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


