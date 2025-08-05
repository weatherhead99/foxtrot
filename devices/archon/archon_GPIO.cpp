#include "archon_GPIO.h"
#include <stdexcept>
#include <string>
#include "archon.h"
#include "archon_modules.h"


using std::string;

foxtrot::devices::archonGPIO::archonGPIO(std::weak_ptr<archon>& arch, const archon_module_info& inf, bool pairwise_direction)
  : _arch_gpio(arch), _inf(inf), _pairwise_direction(pairwise_direction)
{
    
}

bool foxtrot::devices::archonGPIO::getDIOPower()
{
    auto val = readConfigKey_gpio("DIO_POWER");
    return std::stoi(val);
}


string foxtrot::devices::archonGPIO::get_dirstring(int pairorchannel)
{
  std::string out;
  if(_pairwise_direction)
    {
          switch(pairorchannel)
	    {
	    case(1) : out = "12" ; break;
	    case(2) : out = "34" ; break;
	    case(3) : out = "56" ; break;
	    case(4) : out = "78" ; break;
	    default:
	      throw std::out_of_range("invalid value for pair in getDirection");
	    }
    }
  else
    out = std::to_string(pairorchannel);
  return out;

}



								

								
bool foxtrot::devices::archonGPIO::getDirection(int pair)
{
  std::string pairstr = get_dirstring(pair);
    
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

foxtrot::devices::archon_gpio_source foxtrot::devices::archonGPIO::getSource(int ch)
{
    _oss.str("");
    _oss << "DIO_SOURCE" << ch;
    auto val = readConfigKey_gpio(_oss.str());
    
    return static_cast<foxtrot::devices::archon_gpio_source>(std::stoi(val));
    
}

void foxtrot::devices::archonGPIO::setDIOPower(bool onoff)
{
    writeConfigKey_gpio("DIO_POWER", std::to_string(onoff));
}

void foxtrot::devices::archonGPIO::setDirection(int pair, bool inout)
{
  std::string pairstr = get_dirstring(pair);
    
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

void foxtrot::devices::archonGPIO::setSource(int ch, foxtrot::devices::archon_gpio_source source)
{
    _oss.str("");
    _oss << "DIO_SOURCE" << ch;
    writeConfigKey_gpio(_oss.str(), std::to_string(static_cast<short unsigned>(source)));
    
}

int source_to_int(foxtrot::devices::archon_gpio_source source, bool& ok)
{
  ok = true;
    return static_cast<int>(source);
    
};

foxtrot::devices::archon_gpio_source int_to_source(int source, bool& ok)
{
    if(source < 0 || source > 3)
    {
        ok = false;
        return foxtrot::devices::archon_gpio_source::low;
    }
  
  ok = true;  
  return static_cast<foxtrot::devices::archon_gpio_source>(source);
    
};


string foxtrot::devices::archonGPIO::readConfigKey_gpio(const string& subkey)
{
  std::ostringstream oss;
  auto modpos_gpio = _inf.position;
  oss << "MOD" << (modpos_gpio) << "/" << subkey;

  if(auto ptr = _arch_gpio.lock())
    return ptr->readKeyValue(oss.str());
  else
    throw std::logic_error("archon object destroyed, cannot use a submodule of it!");
}

void foxtrot::devices::archonGPIO::writeConfigKey_gpio(const string& key, const string& val)
{
  std::ostringstream oss;
  auto modpos_gpio = _inf.position;
  oss << "MOD" <<  (modpos_gpio) << "/" << key ;

  if(auto ptr = _arch_gpio.lock())
    ptr->writeKeyValue(oss.str(), val);
  else
    throw std::logic_error("archon object destroyed, cannot use a submodule of it!");
}

void foxtrot::devices::archonGPIO::status(foxtrot::devices::archon_module_status& out, const ssmap& statusmap) const
{
  auto findstr = std::format("MOD{}/DINPUTS", _inf.position);
  out.dinput_status = std::stoul(statusmap.at(findstr), nullptr,  2);

}

using foxtrot::devices::archon_gpioprop;
using std::vector;
using foxtrot::devices::archonGPIO;



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::archonGPIO;
 using foxtrot::devices::archon_gpio_source;
 
 type::register_converter_func(int_to_source);
 type::register_converter_func(source_to_int);

 registration::class_<archonGPIO>("foxtrot::devices::archonGPIO")
 .method("setLabel",&archonGPIO::setLabel)
 (parameter_names("ch","label"))
 .method("getLabel",&archonGPIO::getLabel)
 (parameter_names("ch"))
   .property("DIOPower", &archonGPIO::getDIOPower,
	     &archonGPIO::setDIOPower)
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
