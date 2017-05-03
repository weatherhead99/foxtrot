#include "archon_module_xvbias.h"

foxtrot::devices::ArchonXV::ArchonXV(foxtrot::devices::archon& arch, int modpos)
: foxtrot::devices::ArchonLVX(arch,modpos),
_lg("ArchonXV"),
_lcbias(*this,"XVN",4,-95.0,0.,_lg),
_hcbias(*this,"XVP",4,0,90.0,_lg)
{
    
}

const string devices::ArchonXV::getDeviceTypeName() const
{
  return "ArchonXV";
}



std::unique_ptr<foxtrot::devices::ArchonModule> foxtrot::devices::ArchonXV::constructModule(foxtrot::devices::archon& arch, int modpos)
{
    std::unique_ptr<foxtrot::devices::ArchonModule> out(new 
    ArchonXV(arch,modpos));
    
    return out;
}

void foxtrot::devices::ArchonXV::update_variables()
{
    
}

const std::string foxtrot::devices::ArchonXV::getTypeName() const
{
    return "XV";
}
