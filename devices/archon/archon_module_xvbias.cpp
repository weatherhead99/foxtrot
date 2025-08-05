#include "archon_module_xvbias.h"

foxtrot::devices::ArchonXV::ArchonXV(std::weak_ptr<archon>& arch, const archon_module_info& modpos)
: foxtrot::devices::ArchonLVX(arch,modpos)
{
  _lg = Logging("ArchonXV");
  _lcbias.reconfigure("XVN",4,-95.0,0.);
  _hcbias.reconfigure("XVP",4,0,90.0);
  _hasgpio = false;
}

const string devices::ArchonXV::getDeviceTypeName() const
{
  return "ArchonXV";
}


const std::string foxtrot::devices::ArchonXV::getTypeName() const
{
    return "XV";
}
