#include "archon_module_LVDS.hh"


using namespace foxtrot::devices;


using foxtrot::devices::ArchonLVDS;


ArchonLVDS::ArchonLVDS(std::weak_ptr<archon> &arch, const archon_module_info&  modinf)
    : ArchonModule(arch, modinf), archonGPIO(arch, modinf, false) {}

ArchonLVDS::~ArchonLVDS()
{
}

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

const string ArchonLVDS::getTypeName() const
{
  return get_module_name(archon_module_types::LVDS);
}
