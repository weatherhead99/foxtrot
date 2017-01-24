#include "archon.h"
#include "simpleTCP.h"
#include <memory>
#include <string>
#include <iostream>
#include "archon_modules.h"
#include "archon_module_heaterx.h"
#include "TestUtilities.h"
#include "backward.hpp"
#include <DeviceError.h>
#include <vector>
#define BACKWARD_HAS_BFD 1
using std::cout;
using std::endl;


std::vector<string> config_lines
{ {"MOD11/SENSORACURRENT=10000"},
{"MOD11/SENSORALABEL=TANK"},
{"MOD11/SENSORALOWERLIMIT=-150.0"},
{"MOD11/SENSORATYPE=2"},
{"MOD11/SENSORAUPPERLIMIT=50.0"},
{"MOD11/SENSORBCURRENT=10000"},
{"MOD11/SENSORBLABEL=STAGE"},
{"MOD11/SENSORBUPPERLIMIT=50.0"},
{"MOD11/SENSORBLOWERLIMIT=-150.0"},
{"MOD11/SENSORBTYPE=2"},
{"LINES=0"},
{"STATES=0"},
{"PARAMETERS=0"},
{"CONSTANTS=0"}
};


int main(int argc, char** argv)
{
  backward::SignalHandling sh;
  foxtrot::parameterset params;
  params["addr"] = "10.0.0.2";
  params["port"] = 4242u;
  
  std::shared_ptr<foxtrot::protocols::simpleTCP> proto(new foxtrot::protocols::simpleTCP(&params));
  
  std::cout << "init archon... " << std::endl;
  
  foxtrot::devices::archon a(proto);
  
  
  
  auto modules = a.getAllModules();
  for( auto& mod: modules)
  {
    std::cout << "module in position: " << mod.first << std::endl;
    
  };
  
  
  std::cout << "clearing config..." << std::endl;
  a.clear_config();
  
 auto heater = static_cast<foxtrot::devices::ArchonHeaterX*>(&modules.at(10));
 heater->setSensorType(foxtrot::devices::HeaterXSensors::A, foxtrot::devices::HeaterXSensorTypes::RTD100);
  
  
//   std::cout << "attempting to write new config..." << std::endl;
//   for(const auto& line : config_lines)
//   {
//     a.writeConfigLine(line);
//   }
//   
//   cout << "attempting to read config back..." << endl;
//   for(int i=0; i< 10; i++)
//   {
//    cout << a.readConfigLine(i) << endl; 
//   }
//   
//   cout << "applying config..." << endl;
//   try{
//     a.applyall();
//     
//   }
//   catch(foxtrot::DeviceError)
//   {
//     auto all_logs = a.fetch_all_logs();
//     for(auto& log: all_logs)
//     {
//       cout << log << endl;
//     };
//     
//   }
//   
//   auto heater = static_cast<const foxtrot::devices::ArchonHeaterX*>(&modules.at(10));
//   
//   std::cout << "tempA:" << heater->getTempA() << std::endl;
//   std::cout << "tempB:" << heater->getTempB() << std::endl;
//   std::cout << "tempC:" << heater->getTempC() << std::endl;
//   
//   
};
