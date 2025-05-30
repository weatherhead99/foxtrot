#include "archon_module_LVDS.hh"


using namespace foxtrot::devices;


using foxtrot::devices::ArchonLVDS;


ArchonLVDS::ArchonLVDS(archon &arch, short unsigned int modpos)
  : ArchonModule(arch, modpos), archonGPIO(arch, modpos, false)
{}

void ArchonLVDS::setLVDSLabel(int channel, const string& val)
{
  checkChannel(channel);
  writeConfigKey(std::format("LVDS_LABEL{}",channel), val);
}

string ArchonLVDS::getLVDSLabel(int channel)
{
  checkChannel(channel);
  return readConfigKey(std::format("LVDS_LABEL{}", channel));
}
