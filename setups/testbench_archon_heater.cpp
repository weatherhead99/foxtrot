#include "DeviceHarness.h"
#include "devices/archon/archon.h"
#include "devices/archon/archon_modules.h"
#include "devices/archon/archon_module_heaterx.h"

#include "protocols/simpleTCP.h"

#include "protocols/SerialPort.h"
#include "devices/TPG362/TPG362.h"

#include <memory>

foxtrot::parameterset archon_params
{
    {"addr" , "10.0.0.2"},
    {"port" , 4242u},
    {"timeout", 30}
};

foxtrot::parameterset tpg_params {
  {"port" , "/dev/ttyUSB0"},
  {"baudrate" , 9600u},
  };
 


extern "C" { 
int setup(foxtrot::DeviceHarness& harness)
{   
    
    auto archontcp = std::make_shared<foxtrot::protocols::simpleTCP>(&archon_params);
    
    auto archon = std::unique_ptr<foxtrot::devices::archon> (
        new foxtrot::devices::archon(archontcp));
    
    auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&tpg_params);
    
    auto presgauge = std::unique_ptr<foxtrot::devices::TPG362> (new foxtrot::devices::TPG362(sport));
    
    auto modules = archon->getAllModules();
    
    //WARNING: segault ahoy
    auto heater = static_cast<foxtrot::devices::ArchonHeaterX*>(&modules.at(10));
    
    using foxtrot::devices::HeaterXSensors;
    using foxtrot::devices::HeaterXHeaters;
    
    
    heater->setSensorCurrent(HeaterXSensors::A, 50000);
    heater->setSensorCurrent(HeaterXSensors::B, 50000);
    heater->setSensorLowerLimit(HeaterXSensors::A, -150.0);
    heater->setSensorLowerLimit(HeaterXSensors::B, -150.0);
    heater->setSensorUpperLimit(HeaterXSensors::A, 50.0);
    heater->setSensorUpperLimit(HeaterXSensors::B, 50.0);
    heater->setSensorType(HeaterXSensors::A, foxtrot::devices::HeaterXSensorTypes::RTD100);
    heater->setSensorType(HeaterXSensors::B, foxtrot::devices::HeaterXSensorTypes::RTD100);
    heater->setSensorLabel(HeaterXSensors::A, "Tank");
    heater->setSensorLabel(HeaterXSensors::B, "Stage");
    
    
    heater->setHeaterP(HeaterXHeaters::A, 1000);
    heater->setHeaterI(HeaterXHeaters::A,0);
    heater->setHeaterD(HeaterXHeaters::A, 8000);
    heater->setHeaterUpdateTime(2000);
    
    heater->setHeaterSensor(HeaterXHeaters::A, HeaterXSensors::B);
    
    heater->setHeaterTarget(HeaterXHeaters::A, -100.);
    
    archon->applyall();
    
    archon->set_power(true);
    
    
    //TODO: doesn't work right now for some reason???
//     heater->apply();
    
    
    //WARNING: oh shit, a const_cast
    auto heaterptr = std::unique_ptr<foxtrot::Device,
    void(*)(foxtrot::Device*)> 
    (   static_cast<foxtrot::Device*>(
        const_cast<foxtrot::devices::ArchonHeaterX*>(heater)), 
        [] (foxtrot::Device*) {}) ;
    

    harness.AddDevice(std::move(presgauge));
    
    harness.AddDevice(std::move(archon));
    harness.AddDevice(std::move(heaterptr));
    
    return 0;  
};
}
