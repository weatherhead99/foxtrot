#include "archon_module_heaterx.h"
#include <sstream>




devices::ArchonHeaterX::ArchonHeaterX(devices::archon& arch, short unsigned int modpos): ArchonModule(arch, modpos)
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

void devices::ArchonHeaterX::setSensorType(devices::HeaterXSensors sensor, devices::HeaterXSensorTypes type) 
{
    std::ostringstream oss;
    oss << "SENSOR" << static_cast<char>(sensor) << "TYPE";
    
    writeConfigKey(oss.str(),std::to_string(static_cast<short unsigned>(type)));
    
}

devices::HeaterXSensorTypes devices::ArchonHeaterX::getSensorType(devices::HeaterXSensors sensor)
{
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) << "TYPE";
  
  return static_cast<HeaterXSensorTypes>(std::stoul(readConfigKey(oss.str())));
  
}

void devices::ArchonHeaterX::setSensorCurrent(devices::HeaterXSensors sensor, int curr_na)
{
  std::ostringstream oss;
  oss <<"SENSOR" << static_cast<char>(sensor) << "CURRENT";
  
  writeConfigKey(oss.str(),std::to_string(curr_na));
  
}



int devices::ArchonHeaterX::getSensorCurrent(devices::HeaterXSensors sensor)
{
  std::ostringstream oss;
  oss << "SENSOR" << static_cast<char>(sensor) << "CURRENT";
  
  return std::stoi(readConfigKey(oss.str()));

}

