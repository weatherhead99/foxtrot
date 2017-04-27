#include "archon_module_driver.h"

using foxtrot::devices::ArchonModule ;

foxtrot::devices::ArchonDriver::ArchonDriver(foxtrot::devices::archon& arch, unsigned short modpos)
: ArchonModule(arch,modpos)
{
}

std::unique_ptr<ArchonModule> foxtrot::devices::ArchonDriver::constructModule(foxtrot::devices::archon& arch, int modpos)
{
    std::unique_ptr<foxtrot::devices::ArchonModule> out(new ArchonDriver(arch,modpos));
    
    return out;
    
}


double foxtrot::devices::ArchonDriver::getFastSlewRate(int channel)
{
    checkChannel(channel);
    _oss.str("");
    _oss << "FASTSLEWRATE" << channel;
    
    auto slewrate = readConfigKey(_oss.str());
    return std::stod(slewrate);
}


std::string foxtrot::devices::ArchonDriver::getLabel(int channel)
{
    checkChannel(channel);
    _oss.str("");
    _oss << "LABEL" << channel;
    
    auto lab = readConfigKey(_oss.str());
    return lab;
    
}

double foxtrot::devices::ArchonDriver::getSlowSlewRate(int channel)
{
    checkChannel(channel);
    _oss.str("");
    _oss << "SLOWSLEWRATE" << channel;
     
    auto slewrate = readConfigKey(_oss.str());
    return std::stod(slewrate);
}

void foxtrot::devices::ArchonDriver::setFastSlewRate(int channel, double val)
{
    checkChannel(channel);
    _oss.str("");
    _oss << "FASTSLEWRATE" << channel;
    
    if(val < 0.001 || val > 1000)
    {
        throw std::out_of_range("invalid slew rate value");
    }
    
    writeConfigKey(_oss.str(),std::to_string(val));
    
}

void foxtrot::devices::ArchonDriver::setLabel(int channel, const std::string& val)
{
    checkChannel(channel);
    _oss.str("");
    _oss << "LABEL" << channel;
    
    writeConfigKey(_oss.str(),val);
    
}


void foxtrot::devices::ArchonDriver::setSlowSlewRate(int channel, double val)
{
    checkChannel(channel);
    
    _oss.str("");
    _oss << "SLOWSLEWRATE" << channel;
    if(val < 0.001 || val > 1000)
    {
        throw std::out_of_range("invalid slew rate value");
    }
    
    writeConfigKey(_oss.str(), std::to_string(val));
    
}

void foxtrot::devices::ArchonDriver::update_variables()
{
    
}

const string foxtrot::devices::ArchonDriver::getTypeName() const
{
    return "Driver";
}




