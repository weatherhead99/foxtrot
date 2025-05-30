#include "archon_module_lvxbias.h"
#include "archon_modules.h"

foxtrot::devices::ArchonLVX::ArchonLVX(foxtrot::devices::archon& arch, unsigned short modpos)
: foxtrot::devices::ArchonModule(arch,modpos), 
foxtrot::devices::archonGPIO(arch,modpos),
_lg("ArchonLVX"),
_lcbias(*this, "LVLC", 24, -14.0, 14.0,_lg),
_hcbias(*this, "LVHC", 6, -14.0,14.0,_lg)
{
}


const string devices::ArchonLVX::getTypeName() const
{
  
  return string("LVX");

}

const string devices::ArchonLVX::getDeviceTypeName() const
{
    return "ArchonLVX";
}


void devices::ArchonLVX::update_variables()
{
  
  //TODO: update this!
  

}



void foxtrot::devices::ArchonLVX::setLabel(bool HC, int channel, const std::string& label)
{
    if(HC)
    {
        _hcbias.setLabel(channel,label);
    }
    else
    {
        _lcbias.setLabel(channel,label);
    }
    
}

std::string foxtrot::devices::ArchonLVX::getLabel(bool HC, int channel)
{
    if(HC)
    {
        return _hcbias.getLabel(channel);
    }
    return _lcbias.getLabel(channel);
    
}

void foxtrot::devices::ArchonLVX::setOrder(bool HC, int channel, int sequence)
{
    if(HC)
    {
        _hcbias.setOrder(channel,sequence);
    }
    else
    {
        _lcbias.setOrder(channel,sequence);
    }
    
}

int foxtrot::devices::ArchonLVX::getOrder(bool HC, int channel)
{
    if(HC)
    {
        return _hcbias.getOrder(channel);
    }
    return _lcbias.getOrder(channel);
    
    
}

void foxtrot::devices::ArchonLVX::setV(bool HC, int channel, double V)
{
    if(HC)
    {
        _hcbias.setV(channel,V);
    }
    else
    {
        _lcbias.setV(channel, V);
    }
    
}

double foxtrot::devices::ArchonLVX::getV(bool HC, int channel)
{
    if(HC)
    {
        return _hcbias.getV(channel);
    }
    return _lcbias.getV(channel);
    
}

void foxtrot::devices::ArchonLVX::setEnable(bool HC, int channel, bool onoff)
{
    if(HC)
    {
        _hcbias.setEnable(channel,onoff);
    }
    else
    {
        _lcbias.setEnable(channel,onoff);
    }
}

bool foxtrot::devices::ArchonLVX::getEnable(bool HC, int channel)
{
    if(HC)
    {
        return _hcbias.getEnable(channel);
    }
    return _lcbias.getEnable(channel);
}


void foxtrot::devices::ArchonLVX::setLimit(int channel, int limit_mA)
{
    _hcbias.check_channel_number(channel);
    if(limit_mA < 0 || limit_mA > 500)
    {
     throw std::out_of_range("invalid current limit, <0 or > 500");      
    }
    
    std::ostringstream oss;
    oss << "LVHC_IL" << channel;
    
    writeConfigKey(oss.str(),std::to_string(limit_mA));
    
}


int foxtrot::devices::ArchonLVX::getLimit(int channel)
{
    _hcbias.check_channel_number(channel);
    
    std::ostringstream oss;
    oss << "LVHC_IL" << channel;
    return std::stoi(readConfigKey(oss.str()));
    
}


double devices::ArchonLVX::measureI(bool HC, int channel)
{
  if(HC)
  {
    return _hcbias.measureI(channel);
  }
  else
  {
    return _lcbias.measureI(channel);
  }

}



double devices::ArchonLVX::measureV(bool HC, int channel)
{
  if(HC)
  {
    return _hcbias.measureV(channel);
  }
  else
  {
    return _lcbias.measureV(channel);
  }

}


RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::ArchonLVX;

 registration::class_<ArchonLVX>("foxtrot::devices::ArchonLVX")
 .method("setLabel", &ArchonLVX::setLabel)
 (parameter_names("HC", "channel", "label"))
 .method("getLabel", &ArchonLVX::getLabel)
 (parameter_names("HC","channel"))
 .method("setOrder", &ArchonLVX::setOrder)
 (parameter_names("HC","channel","sequence"))
 .method("getOrder", &ArchonLVX::getOrder)
 (parameter_names("HC","channel"))
 .method("setV", &ArchonLVX::setV)
 (parameter_names("HC","channel","V"))
 .method("getV",&ArchonLVX::getV)
 (parameter_names("HC","channel"))
 .method("setEnable",&ArchonLVX::setEnable)
 (parameter_names("HC","channel","onoff"))
 .method("getEnable",&ArchonLVX::getEnable)
 (parameter_names("HC","channel"))
 .method("setLimit",&ArchonLVX::setLimit)
 (parameter_names("channel","limit_mA"))
 .method("getLimit",&ArchonLVX::getLimit)
 .method("measureI",&ArchonLVX::measureI)
 (parameter_names("HC","channel"))
 .method("measureV",&ArchonLVX::measureV)
 (parameter_names("HC","channel"))
 ;
  
  
}


    





