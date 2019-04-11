#include <sstream>
#include <foxtrot/DeviceError.h>
#include <foxtrot/Logging.h>
#include "archon_module_heaterx.h"

using foxtrot::DeviceError;
using namespace foxtrot;
using namespace foxtrot::devices;

template<typename T> void assert_limits(T low, T high, T val)
{
 if(val < low)
 {
   throw DeviceError("value: " + std::to_string(val) + " is below limit: " + std::to_string(low));
 }
 else if (val > high)
 {
   throw DeviceError("value: " + std::to_string(val) + "is above limit: " + std::to_string(high));
 }
  
}

const string devices::ArchonHeaterX::getDeviceTypeName() const
{
  return "ArchonHeaterX";
}



devices::ArchonHeaterX::ArchonHeaterX(devices::archon& arch, short unsigned int modpos): ArchonModule(arch, modpos),
archonGPIO(arch,modpos)
{
  update_variables();
  
}


const string devices::ArchonHeaterX::getTypeName() const
{
  
  return string("HeaterX");

}


double devices::ArchonHeaterX::getHeaterAOutput() const
{
  return _heaterAOutput;

}

double devices::ArchonHeaterX::getHeaterBOutput() const
{
  return _heaterBOutput;

}

double devices::ArchonHeaterX::getTempA() const
{
  return _TempA;
}


double devices::ArchonHeaterX::getTempB() const
{
  return _TempB;
}

double devices::ArchonHeaterX::getTempC() const
{
  return _TempC;
}

int devices::ArchonHeaterX::getHeaterAP() const
{
  return _heaterAP;

}

int devices::ArchonHeaterX::getHeaterAI() const
{
  return _heaterAI;
  
}

int devices::ArchonHeaterX::getHeaterAD() const
{
  return _heaterAD;
}

int devices::ArchonHeaterX::getHeaterBP() const
{
  return _heaterBP;
}


int devices::ArchonHeaterX::getHeaterBI() const
{
  return _heaterBI;
}

int devices::ArchonHeaterX::getHeaterBD() const
{
  return _heaterBD;
}


std::array< bool, int(8) > devices::ArchonHeaterX::getGPIO() const
{
  return _GPIO;

}


void devices::ArchonHeaterX::update_variables()
{
  
  
  auto statmap = _arch.getStatus();
//   try{
  
  _heaterAOutput = extract_module_variable<decltype(_heaterAOutput)>(_modpos,"HEATERAOUTPUT",statmap);
  _heaterBOutput = extract_module_variable<decltype(_heaterBOutput)>(_modpos,"HEATERBOUTPUT",statmap);
  
  _TempA = extract_module_variable<decltype(_TempA)>(_modpos,"TEMPA",statmap);
  _TempB = extract_module_variable<decltype(_TempB)>(_modpos,"TEMPB",statmap);
  _TempC = extract_module_variable<decltype(_TempC)>(_modpos,"TEMPC",statmap);
  
  _heaterAP = extract_module_variable<decltype(_heaterAP)>(_modpos,"HEATERAP",statmap);
  _heaterAI = extract_module_variable<decltype(_heaterAI)>(_modpos,"HEATERAI",statmap);
  _heaterAD = extract_module_variable<decltype(_heaterAD)>(_modpos,"HEATERAD",statmap);
  
  _heaterBP = extract_module_variable<decltype(_heaterBP)>(_modpos,"HEATERBP",statmap);
  _heaterBI = extract_module_variable<decltype(_heaterBI)>(_modpos,"HEATERBI",statmap);
  _heaterBD = extract_module_variable<decltype(_heaterBD)>(_modpos,"HEATERBD",statmap);
  
//   std::cout << "modpos:" << _modpos << std::endl;
  auto gpiostr = get_module_variable_string(_modpos,"DINPUTS",statmap);
  std::istringstream iss(gpiostr);
  std::ostringstream oss;
  
  for(auto& b : _GPIO)
  {
   oss << iss.get();
   b = static_cast<bool>(std::stoi(oss.str()));
   oss.str("");
  }
    
}

std::unique_ptr<foxtrot::devices::ArchonModule> foxtrot::devices::ArchonHeaterX::constructModule
(foxtrot::devices::archon& arch, int modpos)
{
  
 std::unique_ptr<foxtrot::devices::ArchonModule> out(new ArchonHeaterX(arch,modpos));
 
 return out;
}

void ArchonHeaterX::setSensorType(HeaterXSensors sensor, HeaterXSensorTypes type) 
{
    std::ostringstream oss;
    oss << "SENSOR" << static_cast<char>(sensor) << "TYPE";
    
    writeConfigKey(oss.str(),std::to_string(static_cast<short unsigned>(type)));
}

HeaterXSensorTypes devices::ArchonHeaterX::getSensorType(HeaterXSensors sensor)
{
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) << "TYPE";
  
  return static_cast<HeaterXSensorTypes>(std::stoul(readConfigKey(oss.str())));
}

void ArchonHeaterX::setSensorCurrent(HeaterXSensors sensor, int curr_na)
{
  std::ostringstream oss;
  oss <<"SENSOR" << static_cast<char>(sensor) << "CURRENT";
  
  writeConfigKey(oss.str(),std::to_string(curr_na));
}



int ArchonHeaterX::getSensorCurrent(HeaterXSensors sensor)
{
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) << "CURRENT";
  
  return std::stoi(readConfigKey(oss.str()));
}

void ArchonHeaterX::setHeaterTarget(HeaterXHeaters heater, double target)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "TARGET";
  writeConfigKey(oss.str(), std::to_string(target));
}

double ArchonHeaterX::getHeaterTarget(HeaterXHeaters heater)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "TARGET";
  return std::stod(readConfigKey(oss.str()));
}

//HACK: make it work in an emergency
#ifdef NEW_RTTR_API
void ArchonHeaterX::setHeaterTarget(int heater, double target)
{
  bool success;
 auto htr = int_to_heater(heater,success);
  setHeaterTarget(htr,target);
  
}

double ArchonHeaterX::getHeaterTarget(int heater)
{
  bool success;
  auto htr = int_to_heater(heater,success);
  return getHeaterTarget(htr);
};

#endif


void ArchonHeaterX::setHeaterSensor(HeaterXHeaters heater, HeaterXSensors sensor)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "SENSOR";
  
  int sens;
  switch(sensor)
  {
    case(HeaterXSensors::A):
      sens =  0;
      break;
    case(HeaterXSensors::B):
      sens = 1;
      break;
    case(HeaterXSensors::C):
      sens = 2;
      break;
  };
      
  writeConfigKey(oss.str(),std::to_string(sens));
}

devices::HeaterXSensors devices::ArchonHeaterX::getHeaterSensor(devices::HeaterXHeaters heater)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "SENSOR";
  
  auto sens = std::stoi(readConfigKey(oss.str()));

  switch(sens)
  {
  case(0):
   return HeaterXSensors::A;
  case(1):
    return HeaterXSensors::B;
  case(2):
    return HeaterXSensors::C;
  }
  
}


void devices::ArchonHeaterX::setSensorLowerLimit(devices::HeaterXSensors sensor, double temp)
{
  
  assert_limits(-150.,50.,temp);
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) <<"LOWERLIMIT";
  writeConfigKey(oss.str(),std::to_string(temp));
  
}

double devices::ArchonHeaterX::getSensorLowerLimit(devices::HeaterXSensors sensor)
{
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) <<"LOWERLIMIT";
  return std::stod(readConfigKey(oss.str()));

}

void devices::ArchonHeaterX::setSensorUpperLimit(devices::HeaterXSensors sensor, double temp)
{
  assert_limits(-150.,50.,temp);
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) <<"UPPERLIMIT";
  writeConfigKey(oss.str(),std::to_string(temp));

}

double devices::ArchonHeaterX::getSensorUpperLimit(devices::HeaterXSensors sensor)
{
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) <<"UPPERLIMIT";
  return std::stod(readConfigKey(oss.str()));

}

void foxtrot::devices::ArchonHeaterX::setHeaterEnable(foxtrot::devices::HeaterXHeaters heater, bool onoff)
{
    std::ostringstream oss;
    oss << "HEATER" << static_cast<char>(heater) << "ENABLE";
    writeConfigKey(oss.str(),std::to_string(static_cast<int>(onoff)));
    
}

bool foxtrot::devices::ArchonHeaterX::getHeaterEnable(foxtrot::devices::HeaterXHeaters heater)
{
    std::ostringstream oss;
    oss << "HEATER" << static_cast<char>(heater) << "ENABLE";
    return std::stoi(readConfigKey(oss.str()));
    
}



void devices::ArchonHeaterX::setHeaterP(devices::HeaterXHeaters heater, int p)
{
  assert_limits(0,10000,p);
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "P";
  writeConfigKey(oss.str(),std::to_string(p));
}

int devices::ArchonHeaterX::getHeaterP(devices::HeaterXHeaters heater)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "P";
  return std::stoi(readConfigKey(oss.str()));
}


void devices::ArchonHeaterX::setHeaterI(devices::HeaterXHeaters heater, int i)
{
  assert_limits(0,10000,i);
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "I";
  writeConfigKey(oss.str(),std::to_string(i));
}

int devices::ArchonHeaterX::getHeaterI(devices::HeaterXHeaters heater)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "I";
  return std::stoi(readConfigKey(oss.str()));
}

void devices::ArchonHeaterX::setHeaterIL(devices::HeaterXHeaters heater, int il)
{
  assert_limits(0,10000,il);
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "IL";
  writeConfigKey(oss.str(),std::to_string(il));
}

int devices::ArchonHeaterX::getHeaterIL(devices::HeaterXHeaters heater)
{
  
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "IL";
  return std::stoi(readConfigKey(oss.str()));
}

void devices::ArchonHeaterX::setHeaterD(devices::HeaterXHeaters heater, int d)
{
  assert_limits(0,10000,d);
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "D";
  writeConfigKey(oss.str(),std::to_string(d));
}

int devices::ArchonHeaterX::getHeaterD(devices::HeaterXHeaters heater)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "D";
  return std::stoi(readConfigKey(oss.str()));
}

void devices::ArchonHeaterX::setHeaterUpdateTime(int ms)
{
  assert_limits(1,30000,ms);
  writeConfigKey("HEATERUPDATETIME",std::to_string(ms));
}

int devices::ArchonHeaterX::getHeaterUpdateTime()
{
  return std::stoi(readConfigKey("HEATERUPDATETIME"));

}

void devices::ArchonHeaterX::setHeaterRamp(devices::HeaterXHeaters heater, bool onoff)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "RAMP";
  writeConfigKey(oss.str(),std::to_string(static_cast<int>(onoff)));
}

bool devices::ArchonHeaterX::getHeaterRamp(devices::HeaterXHeaters heater)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "RAMP";
  return static_cast<bool>(std::stoi(readConfigKey(oss.str())));
  
}

void devices::ArchonHeaterX::setHeaterLabel(devices::HeaterXHeaters heater, const string& label)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "LABEL";
  writeConfigKey(oss.str(), label);
}

string devices::ArchonHeaterX::getHeaterLabel(devices::HeaterXHeaters heater)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "LABEL";
  return readConfigKey(oss.str());
}

void devices::ArchonHeaterX::setSensorLabel(devices::HeaterXSensors sensor, const string& label)
{
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) << "LABEL";
  writeConfigKey(oss.str(), label);
}

string devices::ArchonHeaterX::getSensorLabel(devices::HeaterXSensors sensor)
{
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) << "LABEL";
  return readConfigKey(oss.str());
}

void devices::ArchonHeaterX::setHeaterRampRate(devices::HeaterXHeaters heater, int rate_mk_ut)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "RAMPRATE";
  writeConfigKey(oss.str(),std::to_string(rate_mk_ut));
}

int devices::ArchonHeaterX::getHeaterRampRate(devices::HeaterXHeaters heater)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "RAMPRATE";
  return std::stoi(readConfigKey(oss.str()));
}

double ArchonHeaterX::getHeaterLimit(HeaterXHeaters heater)
{
  std::ostringstream oss;
  oss <<"HEATER" << static_cast<char>(heater) << "LIMIT";
  return std::stoi(readConfigKey(oss.str()));

}

void ArchonHeaterX::setHeaterLimit(HeaterXHeaters heater, double lim)
{
  std::ostringstream oss;
  oss << "HEATER" << static_cast<char>(heater) << "LIMIT";
  writeConfigKey(oss.str(),std::to_string(lim));

}



int foxtrot::devices::heater_to_int(devices::HeaterXHeaters heater, bool& success)
{
    success = true;
    switch(heater)
    {
        case(devices::HeaterXHeaters::A):
            return 0;
        case(devices::HeaterXHeaters::B):
            return 1;
    }
    
}

devices::HeaterXHeaters foxtrot::devices::int_to_heater(int i, bool& success)
{
  
    success = true;
    switch(i)
    {
        case(0):
            return devices::HeaterXHeaters::A;
        case(1):
            return devices::HeaterXHeaters::B;
        default:
            success = false;
            return devices::HeaterXHeaters::A;
    }
    
}

int foxtrot::devices::sensor_to_int(devices::HeaterXSensors sensor, bool& success)
{
    success = true;
    switch(sensor)
    {
        case(devices::HeaterXSensors::A):
            return 0;
        case(devices::HeaterXSensors::B):
            return 1;
        case(devices::HeaterXSensors::C):
            return 2;
    }
    
}

devices::HeaterXSensors foxtrot::devices::int_to_sensor(int i, bool& success)
{
    success = true;
    switch(i)
    {
        case(0):
            return devices::HeaterXSensors::A;
        case(1):
            return devices::HeaterXSensors::B;
        case(2):
            return devices::HeaterXSensors::C;
        default:
            success = false;
            return devices::HeaterXSensors::A;
        
    }
    
}

RTTR_REGISTRATION
{
 using namespace rttr;
 using devices::ArchonHeaterX;

 type::register_converter_func(foxtrot::devices::int_to_heater);
 type::register_converter_func(foxtrot::devices::heater_to_int);
 type::register_converter_func(foxtrot::devices::int_to_sensor);
 type::register_converter_func(foxtrot::devices::sensor_to_int);
 
 registration::class_<ArchonHeaterX>("foxtrot::devices::ArchonHeaterX")
 .property_readonly("getHeaterAOutput",&ArchonHeaterX::getHeaterAOutput)
 .property_readonly("getHeaterBOutput",&ArchonHeaterX::getHeaterBOutput)
 .property_readonly("getHeaterAP", &ArchonHeaterX::getHeaterAP)
 .property_readonly("getHeaterAD", &ArchonHeaterX::getHeaterAD)
 .property_readonly("getHeaterAI", &ArchonHeaterX::getHeaterAI)
 .property_readonly("getHeaterBP", &ArchonHeaterX::getHeaterBP)
 .property_readonly("getHeaterBI", &ArchonHeaterX::getHeaterBI)
 .property_readonly("getHeaterBD", &ArchonHeaterX::getHeaterBD)
 .property_readonly("getTempA", &ArchonHeaterX::getTempA)
 .property_readonly("getTempB", &ArchonHeaterX::getTempB)
 .property_readonly("getTempC", &ArchonHeaterX::getTempC)
 .method("setSensorType", &ArchonHeaterX::setSensorType)
 (
     parameter_names("sensor","type")
     )
 .method("getSensorType",&ArchonHeaterX::getSensorType)
 (
     parameter_names("sensor")
     )
 .method("setSensorCurrent",&ArchonHeaterX::setSensorCurrent)
 (
     parameter_names("sensor","curr_na")
     )
 .method("getSensorCurrent",&ArchonHeaterX::getSensorCurrent)
 ( 
    parameter_names("sensor")
    )
#ifdef NEW_RTTR_API
 .method("setHeaterTarget",static_cast<void(ArchonHeaterX::*)(int,double)>(&ArchonHeaterX::setHeaterTarget))
 (
     parameter_names("heater","target")
     )
 .method("getHeaterTarget",static_cast<double(ArchonHeaterX::*)(int)>(&ArchonHeaterX::getHeaterTarget))
 (
     parameter_names("heater")
     )
#else
 .method("setHeaterTarget",&ArchonHeaterX::setHeaterTarget)
 (
     parameter_names("heater","target")
     )
 .method("getHeaterTarget",&ArchonHeaterX::getHeaterTarget)
 (
     parameter_names("heater")
     )
#endif
 .method("setHeaterSensor",&ArchonHeaterX::setHeaterSensor)
 (
     parameter_names("heater","sensor")
     )
 .method("getHeaterSensor",&ArchonHeaterX::getHeaterSensor)
 (
     parameter_names("heater")
     )
 .method("setSensorLowerLimit",&ArchonHeaterX::setSensorLowerLimit)
 (
     parameter_names("sensor","temp")
     )
 .method("getSensorLowerLimit",&ArchonHeaterX::getSensorLowerLimit)
 (
     parameter_names("sensor")
     )
 .method("setSensorUpperLimit",&ArchonHeaterX::setSensorUpperLimit)
 (
     parameter_names("sensor","teno")
     )
 .method("setHeaterP",&ArchonHeaterX::setHeaterP)
 (
     parameter_names("heater","p")
     )
 .method("setHeaterI",&ArchonHeaterX::setHeaterI)
 (
     parameter_names("heater","i")
     )
 .method("setHeaterIL",&ArchonHeaterX::setHeaterIL)
 (
     parameter_names("heater","il")
     )
 .method("setHeaterD",&ArchonHeaterX::setHeaterD)
 (
     parameter_names("heater","d")
     )
 .method("getHeaterP",&ArchonHeaterX::getHeaterP)
 (
     parameter_names("heater")
     )
 .method("getHeaterI",&ArchonHeaterX::getHeaterI)
 (
     parameter_names("heater")
     )
 .method("getHeaterIL",&ArchonHeaterX::getHeaterIL)
 (
     parameter_names("heater")
     )
 .method("getHeaterD",&ArchonHeaterX::getHeaterD)
 (
     parameter_names("heater")
     )
 .method("setHeaterUpdateTime",&ArchonHeaterX::setHeaterUpdateTime)
 (
     parameter_names("ms")
     )
 .property_readonly("getHeaterUpdateTime",&ArchonHeaterX::getHeaterUpdateTime)
 .method("setHeaterRamp",&ArchonHeaterX::setHeaterRamp)
 (
     parameter_names("heater","onoff")
     )
 .method("getHeaterRamp",&ArchonHeaterX::getHeaterRamp)
 (
     parameter_names("heater")
     )
 .method("setHeaterRampRate", &ArchonHeaterX::setHeaterRampRate)
 (
     parameter_names("heater","rate_mk_ut")
     )
 .method("getHeaterRampRate", &ArchonHeaterX::getHeaterRampRate)
 (
     parameter_names("heater")
     )
 .method("setHeaterLabel",&ArchonHeaterX::setHeaterLabel)
 (
     parameter_names("heater","label")
     )
 .method("getHeaterLabel",&ArchonHeaterX::getHeaterLabel)
 (
     parameter_names("heater")
     )
 .method("setSensorLabel",&ArchonHeaterX::setSensorLabel)
 (
     parameter_names("heater","label")
     )
 .method("getSensorLabel",&ArchonHeaterX::getSensorLabel)
 (
     parameter_names("heater")
     )
 .method("setHeaterEnable", &ArchonHeaterX::setHeaterEnable)
 (
     parameter_names("heater","onoff")
     )
 .method("getHeaterEnable", &ArchonHeaterX::getHeaterEnable)
 (
     parameter_names("heater")
     )
 ;
 
     
    
}


