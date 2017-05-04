#include "archon_GPIO.h"
#include <stdexcept>
#include "archon.h"


using std::string;

foxtrot::devices::archonGPIO::archonGPIO(foxtrot::devices::archon& arch, int modpos)
: _arch_gpio(arch), _modpos_gpio(modpos)
{
    
}

bool foxtrot::devices::archonGPIO::getDIOPower()
{
    auto val = readConfigKey_gpio("DIO_POWER");
    return std::stoi(val);
}

bool foxtrot::devices::archonGPIO::getDirection(int pair)
{
    std::string pairstr;
    switch(pair)
    {
        case(1) : pairstr = "12" ; break;
        case(2) : pairstr = "34" ; break;
        case(3) : pairstr = "56" ; break;
        case(4) : pairstr = "78" ; break;
        default:
            throw std::out_of_range("invalid value for pair in getDirection");
    }
    
    _oss.str("");
    _oss << "DIO_DIR" << pairstr; 
    
    auto val = readConfigKey_gpio(_oss.str());
    return std::stoi(val);
    
}

string foxtrot::devices::archonGPIO::getLabel(int ch)
{
    _oss.str("");
    _oss << "DIO_LABEL" << ch;
    return readConfigKey_gpio(_oss.str());
    
}

foxtrot::devices::gpio_source foxtrot::devices::archonGPIO::getSource(int ch)
{
    _oss.str("");
    _oss << "DIO_SOURCE" << ch;
    auto val = readConfigKey_gpio(_oss.str());
    
    return static_cast<foxtrot::devices::gpio_source>(std::stoi(val));
    
}

void foxtrot::devices::archonGPIO::setDIOPower(bool onoff)
{
    writeConfigKey_gpio("DIO_POWER", std::to_string(onoff));
}

void foxtrot::devices::archonGPIO::setDirection(int pair, bool inout)
{
    std::string pairstr;
    switch(pair)
    {
        case(1) : pairstr = "12" ; break;
        case(2) : pairstr = "34" ; break;
        case(3) : pairstr = "56" ; break;
        case(4) : pairstr = "78" ; break;
        default:
            throw std::out_of_range("invalid value for pair in getDirection");
    }
    
    _oss.str("");
    _oss << "DIO_DIR" << pairstr; 
    
    writeConfigKey_gpio(_oss.str(), std::to_string(inout));
    
}

void foxtrot::devices::archonGPIO::setLabel(int ch, const std::string& label)
{
    _oss.str("");
    _oss << "DIO_LABEL" << ch;
    writeConfigKey_gpio(_oss.str(),label);
}

void foxtrot::devices::archonGPIO::setSource(int ch, foxtrot::devices::gpio_source source)
{
    _oss.str("");
    _oss << "DIO_SOURCE" << ch;
    writeConfigKey_gpio(_oss.str(), std::to_string(static_cast<short unsigned>(source)));
    
}

int source_to_int(foxtrot::devices::gpio_source source, bool& ok)
{
  ok = true;
    return static_cast<int>(source);
    
};

foxtrot::devices::gpio_source int_to_source(int source, bool& ok)
{
    if(source < 0 || source > 3)
    {
        ok = false;
        return foxtrot::devices::gpio_source::low;
    }
  
  ok = true;  
  return static_cast<foxtrot::devices::gpio_source>(source);
    
};


string foxtrot::devices::archonGPIO::readConfigKey_gpio(const string& subkey)
{
  std::ostringstream oss;
  oss << "MOD" << (_modpos_gpio+1) << "/" << subkey;
  
  return _arch_gpio.readKeyValue(oss.str());
  
}

void foxtrot::devices::archonGPIO::writeConfigKey_gpio(const string& key, const string& val)
{
  std::ostringstream oss;
  oss << "MOD" <<  (_modpos_gpio+1) << "/" << key ;
  
  _arch_gpio.writeKeyValue(oss.str(),val);

}


RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::archonGPIO;
 
 type::register_converter_func(int_to_source);
 type::register_converter_func(source_to_int);
 
 registration::class_<archonGPIO>("foxtrot::devices::archonGPIO")
 .method("setLabel",&archonGPIO::setLabel)
 (parameter_names("ch","label"))
 .method("getLabel",&archonGPIO::getLabel)
 (parameter_names("ch"))
 .property_readonly("getDIOPower", &archonGPIO::getDIOPower)
 .method("setDIOPower", &archonGPIO::setDIOPower)
 (parameter_names("onoff"))
 .method("setDirection",&archonGPIO::setDirection)
 (parameter_names("pair","inout"))
 .method("getDirection", &archonGPIO::getDirection)
 (parameter_names("pair"))
 .method("setSource",&archonGPIO::setSource)
 (parameter_names("ch","source"))
 .method("getSource",&archonGPIO::getSource)
 (parameter_names("ch"))
 
 ;
    
}
