#include "DeviceHarness.h"
#include "devices/archon/archon.h"
#include "devices/archon/archon_modules.h"
#include "devices/archon/archon_module_heaterx.h"

#include "protocols/simpleTCP.h"

#include <memory>

foxtrot::parameterset archon_params
{
    {"addr" , "10.0.0.2"},
    {"port" , 4242u},
    {"timeout", 30}
};


int setup(foxtrot::DeviceHarness& harness)
{   
    
    auto archontcp = std::make_shared<foxtrot::protocols::simpleTCP>(&archon_params);
    
    auto archon = std::unique_ptr<foxtrot::devices::archon> (
        new foxtrot::devices::archon(archontcp));
    
    
    auto modules = archon->getAllModules();
    
    //WARNING: segault ahoy
    auto heater = static_cast<const foxtrot::devices::ArchonHeaterX*>(&modules.at(10));
    
    //WARNING: oh shit, a const_cast
    auto heaterptr = std::unique_ptr<foxtrot::Device,
    void(*)(foxtrot::Device*)> 
    (   static_cast<foxtrot::Device*>(
        const_cast<foxtrot::devices::ArchonHeaterX*>(heater)), 
        [] (foxtrot::Device*) {}) ;
    
    
    
    harness.AddDevice(std::move(archon));
    harness.AddDevice(std::move(heaterptr));
    
    return 0;  
};
