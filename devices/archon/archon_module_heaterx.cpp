#include "archon_module_heaterx.h"
#include <sstream>

devices::ArchonHeaterX::ArchonHeaterX(devices::archon& arch, short unsigned int modpos): ArchonModule(arch, modpos)
{

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
  
  auto gpiostr = get_module_variable_string(_modpos,"GPIO",statmap);
  std::istringstream iss(gpiostr);
  std::ostringstream oss;
  
  for(auto& b : _GPIO)
  {
   oss << iss.get();
   b = static_cast<bool>(std::stoi(oss.str()));
   oss.clear();
   
  }
    
}
