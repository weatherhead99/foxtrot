#include "testbench_setup_funcs.h"

#include <foxtrot/devices/archon_module_heaterx.h>
#include <foxtrot/devices/archon_module_hvxbias.h>
#include <foxtrot/devices/archon_module_lvxbias.h>
#include <foxtrot/devices/archon_module_xvbias.h>
#include <foxtrot/devices/archon_module_driver.h>
#include <foxtrot/devices/archon_module_AD.h>

void setup_heaterX(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg)
{
    //============== Archon Heater ====================//
    lg.Info("setting up ArchonHeaterX module...");
    auto heater = static_cast<foxtrot::devices::ArchonHeaterX*>(&modules.at(10));
    
    using foxtrot::devices::HeaterXSensors;
    using foxtrot::devices::HeaterXHeaters;
    using foxtrot::devices::gpio_source;
    
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
    
    heater->setHeaterRamp(HeaterXHeaters::A, false);
    heater->setHeaterRampRate(HeaterXHeaters::A, 1);
    
    heater->setHeaterSensor(HeaterXHeaters::A, HeaterXSensors::B);
    heater->setHeaterLabel(HeaterXHeaters::A, "stage");
    
    heater->setHeaterTarget(HeaterXHeaters::A, -95.);
    heater->setHeaterLimit(HeaterXHeaters::A, 25.);
    
    heater->setHeaterEnable(HeaterXHeaters::A,true);
    
    heater->setSource(1,gpio_source::clocked);
    heater->setLabel(1,"PCLAMP1");
    heater->setSource(2,gpio_source::clocked);
    heater->setLabel(1,"PCLAMP2");
    heater->setSource(3,gpio_source::clocked);
    heater->setLabel(1,"PCLAMP3");
    heater->setSource(4,gpio_source::clocked);
    heater->setLabel(1,"PCLAMP4");
    
    heater->setDirection(1,true);
    heater->setDirection(2,true);
    
    heater->setDIOPower(true);
    
    heater->apply();

    
    auto heaterptr = get_ptr_for_harness(heater);
    harness.AddDevice(std::move(heaterptr));
  
};


void setup_hvxbias(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg)
{

    lg.Info("setting up Archon HVXBias module...");
    auto hvxbias = static_cast<foxtrot::devices::ArchonHVX*>(&modules.at(8));
    
    for(int i=1; i<=16; i++)
    {
      hvxbias->setLabel(false,i,"OD" + std::to_string(i));  
      hvxbias->setOrder(false,i,2);
    }
    hvxbias->setLabel(false,17,"RDA");
    hvxbias->setOrder(false,17,3);
    hvxbias->setLabel(false,19,"RDB");
    hvxbias->setOrder(false,19,3);
    
  
    for(int i=1; i<=4; i++)
    {
      hvxbias->setLabel(false,i + 20,"GD" + std::to_string(i));
      hvxbias->setOrder(false,i + 20, 4);
    }
  
    auto hvxptr = get_ptr_for_harness(hvxbias);
    
    harness.AddDevice(std::move(hvxptr));
}


void setup_lvxbias(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset)
{
      
    lg.Info("setting up Archon LVXBias module...");
    
    auto lvxbias = static_cast<foxtrot::devices::ArchonLVX*>(&modules.at(3));
    
    
    for(int i=1; i<=4; i++)
    {
      lvxbias->setLabel(false,i,"OG" + std::to_string(i));
      lvxbias->setOrder(false,i,5);
    }
    
      
    lvxbias->setLabel(true,1,"7V5");
    lvxbias->setLabel(true,2,"m7V5");
    lvxbias->setLabel(true,3,"13V5");
    lvxbias->setLabel(true,5,"m13V5");
    
    const int PREAMP_CURRENT_LIMIT_MA = 350;

    for(int i=1 ;i <= 4; i++)
    {
    lvxbias->setLimit(i,PREAMP_CURRENT_LIMIT_MA);
      if(archon_reset)
      {
	lvxbias->setEnable(true,i,true);
      }
    lvxbias->setOrder(true,i,0);
    }
          
      if(archon_reset)
      {
	lvxbias->setV(true,1,7.5);
	lvxbias->setV(true,2,-7.5);
	lvxbias->setV(true,3,7.5);
	lvxbias->setV(true,4,-7.5);
      }
      
    
    auto lvxptr = get_ptr_for_harness(lvxbias);
    harness.AddDevice(std::move(lvxptr));

};

void setup_xvbias(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset)
{
      lg.Info("setting up Archon XVBias module...");
    
    auto xvbias = static_cast<foxtrot::devices::ArchonXV*>(&modules.at(1));
    
    xvbias->setLabel(false,1,"VBB");
    
    auto xvptr = get_ptr_for_harness(xvbias);
    harness.AddDevice(std::move(xvptr));
   
};


void setup_clockdriver(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset)
{
  
    lg.Info("setting up archon Clock Driver Module 1");
    
    auto clockdriver = static_cast<foxtrot::devices::ArchonDriver*>(&modules.at(9));
    
    clockdriver->setLabel(1,"IPHI1");
    clockdriver->setLabel(2,"IPHI2");
    clockdriver->setLabel(3,"IPHI3");
    clockdriver->setLabel(4,"IPHI4");
    clockdriver->setLabel(5,"RPHI1");
    clockdriver->setLabel(6,"RPHI2");
    clockdriver->setLabel(7,"RPHI3");
    clockdriver->setLabel(8,"PHIR");
    
    if(archon_reset)
    {
      for(int i=1; i<=8; i++)
      {
	clockdriver->setEnable(i,true); 
      }
    }
      
    auto cdptr = get_ptr_for_harness(clockdriver);
    clockdriver->setDeviceComment("CCD_clocks");
    harness.AddDevice(std::move(cdptr));
    
  
}

void setup_clockdriver2(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset)
{
      lg.Info("setting up Archon Clock Driver Module 2");
    
    auto clockdriver2 = static_cast<foxtrot::devices::ArchonDriver*>(&modules.at(2));
    auto cdptr2 = get_ptr_for_harness(clockdriver2);
    clockdriver2->setDeviceComment("spare_clocks");
    
    clockdriver2->setLabel(3,"EXTLINESYNC");
    clockdriver2->setLabel(4,"PMINTEGRATE");
    clockdriver2->setLabel(5,"LEDTRIGGER");
    clockdriver2->setLabel(6,"CT_INJECT");
    if(archon_reset)
    { 
      for (int chan = 3; chan <=7; chan++)
      {
	clockdriver2->setEnable(chan,true);
	clockdriver2->setFastSlewRate(chan,100);
	clockdriver2->setSlowSlewRate(chan,100);
      };
    }
    
    harness.AddDevice(std::move(cdptr2));
}

void setup_ADCs(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset)
{
  
      lg.Info("setting up Archon ADC Modules...");
    auto AD1 = static_cast<foxtrot::devices::ArchonAD*>(&modules.at(4));
    auto ad1ptr = get_ptr_for_harness(AD1);
    AD1->setDeviceComment("AD1");
    
    if(archon_reset)
    {
      AD1->setPreampGain(false);
    }
    harness.AddDevice(std::move(ad1ptr));
    
    auto AD2 = static_cast<foxtrot::devices::ArchonAD*>(&modules.at(5));
    auto ad2ptr = get_ptr_for_harness(AD2);
    AD2->setDeviceComment("AD2");
    
    if(archon_reset)
    {
      AD2->setPreampGain(false); 
    }
    
    harness.AddDevice(std::move(ad2ptr));
    
    auto AD3 = static_cast<foxtrot::devices::ArchonAD*>(&modules.at(6));
    auto ad3ptr = get_ptr_for_harness(AD3);
    AD3->setDeviceComment("AD3");
    
    if(archon_reset)
    {    
      AD3->setPreampGain(false);
    }
    
    harness.AddDevice(std::move(ad3ptr));
    
    auto AD4 = static_cast<foxtrot::devices::ArchonAD*>(&modules.at(7));
    auto ad4ptr = get_ptr_for_harness(AD4);
    AD4->setDeviceComment("AD4");
    
    if(archon_reset)
    {
      AD4->setPreampGain(false);
    }
    
    harness.AddDevice(std::move(ad4ptr));

}
