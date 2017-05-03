#include "archon_module_hvxbias.h"

std::unique_ptr< devices::ArchonModule > devices::ArchonHVX::constructModule(devices::archon& arch, int modpos)
{
  std::unique_ptr<foxtrot::devices::ArchonModule> out(new ArchonHVX(arch,modpos));
  return out;
  

}

const string devices::ArchonHVX::getDeviceTypeName() const
{
  return "ArchonHVX";
}



const string devices::ArchonHVX::getTypeName() const
{
  return string("HVX");

}

void devices::ArchonHVX::update_variables()
{

}


devices::ArchonHVX::ArchonHVX(devices::archon& arch, short unsigned int modpos)
: foxtrot::devices::ArchonLVX(arch,modpos)
{
  _lg = Logging("ArchonHVX");
  _lcbias.reconfigure("HVLC",24,0,31.0);
  _hcbias.reconfigure("HVHC",5,0,31.0);
  
}




