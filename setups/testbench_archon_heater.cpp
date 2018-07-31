#include "DeviceHarness.h"
#include "devices/archon/archon.h"
#include "devices/archon/archon_modules.h"
#include "devices/archon/archon_module_heaterx.h"
#include "devices/archon/archon_module_hvxbias.h"
#include "devices/archon/archon_module_driver.h"
#include "devices/archon/archon_module_AD.h"
#include "devices/archon/archon_module_xvbias.h"
#include "devices/archon/archonraw.h"


#include "protocols/simpleTCP.h"
#include "DeviceError.h"
#include "protocols/SerialPort.h"
#include <BulkUSB.h>
#include "devices/TPG362/TPG362.h"

#include "devices/newport_2936R/newport2936R.h"

#include "devices/cornerstone_260/cornerstone260.h"

#include "devices/OPS-Q250/Q250.h"
#include "devices/stellarnet/stellarnet.h"

#include <memory>
#include <map>
#include <boost/variant.hpp>
#include "Logging.h"

using mapofparametersets = std::map<std::string, foxtrot::parameterset>;


template <typename T> std::unique_ptr<foxtrot::Device, void(*)(foxtrot::Device*)>
get_ptr_for_harness( const T* ptr)
{
  return std::unique_ptr<foxtrot::Device,void(*)(foxtrot::Device*)>
    (static_cast<foxtrot::Device*>(
      const_cast<T*>(ptr)),[](foxtrot::Device*){});
}

extern "C" { 
int setup(foxtrot::DeviceHarness& harness, const mapofparametersets* const params)
{
    foxtrot::Logging lg("setup");
  
    if(params == nullptr)
    {
      lg.Fatal("no parametersets received, cannot continue..");
      throw std::runtime_error("setup cannot continue");
    }
    
    auto setup_params = params->at("setup");
    
    auto archon_reset = static_cast<bool>(boost::get<int>(setup_params.at("archon_reset")));
    
    
    lg.Info("setting up Archon....");
    auto archon_params = params->at("archon_params");
    auto archontcp = std::make_shared<foxtrot::protocols::simpleTCP>(&archon_params);
    
//     auto archon = std::unique_ptr<foxtrot::devices::archon> (
//         new foxtrot::devices::archon(archontcp));
//     
    auto archon = std::unique_ptr<foxtrot::devices::archonraw> (
	new foxtrot::devices::archonraw(archontcp));
    
    
    archon->settrigoutpower(true);
    
    lg.Info("setting up TPG pressure gauge...");
    auto tpg_params = params->at("tpg_params");
    auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&tpg_params);
    
    auto presgauge = std::unique_ptr<foxtrot::devices::TPG362> (new foxtrot::devices::TPG362(sport));
    harness.AddDevice(std::move(presgauge));
    auto modules = archon->getAllModules();
    
    
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
    //============Archon biases & drivers================//
    lg.Info("setting up Archo HVXBias module...");
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
    
    for(int i=1 ;i <= 4; i++)
    {
    lvxbias->setLimit(i,300);
      if(archon_reset)
      {
	lvxbias->setEnable(true,i,true);
      }
    lvxbias->setOrder(true,i,0);
    }
    
      lvxbias->setLimit(1,300);
      lvxbias->setLimit(2,300);
      lvxbias->setLimit(3,300);
      lvxbias->setLimit(4,300);
      
      if(archon_reset)
      {
	lvxbias->setV(true,1,7.5);
	lvxbias->setV(true,2,-7.5);
	lvxbias->setV(true,3,7.5);
	lvxbias->setV(true,4,-7.5);
      }
      
    
    auto lvxptr = get_ptr_for_harness(lvxbias);
    harness.AddDevice(std::move(lvxptr));
    
    lg.Info("setting up Archon XVBias module...");
    
    auto xvbias = static_cast<foxtrot::devices::ArchonXV*>(&modules.at(1));
    
    xvbias->setLabel(false,1,"VBB");
    
    auto xvptr = get_ptr_for_harness(xvbias);
    harness.AddDevice(std::move(xvptr));
    
    
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
    
    //==============Archon A/Ds =====================//
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
    
    if(archon_reset)
    {
      try{  
	
      archon->applyall();
      }
      catch(class foxtrot::DeviceError& err)
      {
	auto archon_logs = archon->fetch_all_logs();
	
	for(auto& log : archon_logs)
	{
	  std::cout << "archon log: " << log << std::endl;
	};
	
	throw err;
	
      }
    }
    
        
    
    harness.AddDevice(std::move(archon));
    
    //===================radiometry system========================//
    lg.Info("setting up Stellarnet Spectrometer");
    auto firmware_file = boost::get<std::string>(setup_params.at("stellarnet_firmware"));

    auto spectrometer = std::unique_ptr<foxtrot::devices::stellarnet>(new foxtrot::devices::stellarnet(firmware_file,1000));
    harness.AddDevice(std::move(spectrometer));
    
    
    //setup power meter
    lg.Info("setting up Newport 2936R power meter...");
    auto newport_params = params->at("newport_params_serial");
    auto powermeterserial = std::make_shared<foxtrot::protocols::SerialPort>(&newport_params);
    
//     auto powermeterusb = std::make_shared<foxtrot::protocols::BulkUSB>(&newport_params);
    auto powermeter = std::unique_ptr<foxtrot::devices::newport2936R>(new foxtrot::devices::newport2936R(powermeterserial));
    
    powermeter->setChannel(1);
    powermeter->setMode(foxtrot::devices::powermodes::Integrate);
    powermeter->setExternalTriggerMode(1);
    powermeter->setTriggerEdge(1);
    
    harness.AddDevice(std::move(powermeter));
    
    
    
    //====================illumination system========================//
    //setup monochromator
    lg.Info("setting up Cornerstone 260 monochromator");
    auto cornerstone_params = params->at("cornerstone_params");
    auto cornerstone_serial = std::make_shared<foxtrot::protocols::SerialPort>(&cornerstone_params);
    auto monoch = std::unique_ptr<foxtrot::devices::cornerstone260>(new foxtrot::devices::cornerstone260(cornerstone_serial));
    
    //calibrate monochromator - TODO do from configuration file somewhere
    monoch->setGratingCalibration(1,600,0.99510,0.0872665,0.086534,"g1");
    monoch->setGratingCalibration(2,600,1.000800,3.22885911,0.085817,"g2");
    monoch->setGratingCalibration(3,600,0.0000,3.22885911,0.000,"");
    harness.AddDevice(std::move(monoch));

    lg.Info("setting up Newport Q250 Power Supply");
    auto psu_params = params->at("psu_params");
    auto scsiser = std::make_shared<foxtrot::protocols::scsiserial>(&psu_params);
    auto lamp_psu = std::unique_ptr<foxtrot::devices::Q250>(new foxtrot::devices::Q250(scsiser));
    
    harness.AddDevice(std::move(lamp_psu));
    
    
    return 0;  
};
}
