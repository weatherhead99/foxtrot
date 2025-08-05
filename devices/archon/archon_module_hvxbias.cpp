#include "archon_module_hvxbias.h"

const string devices::ArchonHVX::getDeviceTypeName() const
{
  return "ArchonHVX";
}


const string devices::ArchonHVX::getTypeName() const
{
  return string("HVX");

}



devices::ArchonHVX::ArchonHVX(std::weak_ptr<archon>& arch, const archon_module_info& modinf)
: foxtrot::devices::ArchonLVX(arch, modinf)
{
  _lg = Logging("ArchonHVX");
  _lcbias.reconfigure("HVLC",24,0,31.0);
  _hcbias.reconfigure("HVHC",5,0,31.0);
  _hasgpio = false;
}




