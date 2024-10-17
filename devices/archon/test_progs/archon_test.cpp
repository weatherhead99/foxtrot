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
  params["timeout"] = 30;
  
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
  
  std::cout << "getting heater module..." << std::endl;
  
 auto heater = static_cast<foxtrot::devices::ArchonHeaterX*>(&modules.at(10));
 
 cout << "setting sensor type..>" << std::endl;
 heater->setSensorType(foxtrot::devices::HeaterXSensors::A, foxtrot::devices::HeaterXSensorTypes::RTD100);
  
//  cout << "getting sensor type..." << std::endl;
//  auto senstype = heater->getSensorType(foxtrot::devices::HeaterXSensors::A);
//  
//  cout << "setting sensor A current..." << std::endl;
//  heater->setSensorCurrent(foxtrot::devices::HeaterXSensors::A,10000);
//  
//  cout << "getting sensor A current..." << std::endl;
//  auto current = heater->getSensorCurrent(foxtrot::devices::HeaterXSensors::A);
//  cout << current << endl;
//  
 a.set_timing_lines(0);
 a.set_states(0);
 a.set_constants(0);
 a.set_parameters(0);
 
 cout << "applying config... " << endl;
 try{
   
  a.applyall();
 }
 catch(foxtrot::DeviceError& err)
 {
   cout << "archon log:" << endl;
   auto logs = a.fetch_all_logs();
   for(auto& log : logs)
   {
     cout << log << endl;
   };
   
 };
};
