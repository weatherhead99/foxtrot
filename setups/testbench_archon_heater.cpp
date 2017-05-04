#include "DeviceHarness.h"
#include "devices/archon/archon.h"
#include "devices/archon/archon_modules.h"
#include "devices/archon/archon_module_heaterx.h"
#include "devices/archon/archon_module_hvxbias.h"
#include "devices/archon/archon_module_driver.h"
#include "devices/archon/archon_module_AD.h"
#include "devices/archon/archon_module_xvbias.h"

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
 
  
foxtrot::parameterset cornerstone_params
{
  {"port", "/dev/ttyS0"}
};
  

const foxtrot::parameterset psu_params
{
  {"devnode", "/dev/sdb"},
  {"timeout", 2000u}
};


//     auto heaterptr = std::unique_ptr<foxtrot::Device,
//     void(*)(foxtrot::Device*)> 
//     (   static_cast<foxtrot::Device*>(
//         const_cast<foxtrot::devices::ArchonHeaterX*>(heater)), 
//         [] (foxtrot::Device*) {}) ;
//     


template <typename T> std::unique_ptr<foxtrot::Device, void(*)(foxtrot::Device*)>
get_ptr_for_harness( const T* ptr)
{
  return std::unique_ptr<foxtrot::Device,void(*)(foxtrot::Device*)>
    (static_cast<foxtrot::Device*>(
      const_cast<T*>(ptr)),[](foxtrot::Device*){});
}



extern "C" { 
int setup(foxtrot::DeviceHarness& harness)
{   
    
    auto archontcp = std::make_shared<foxtrot::protocols::simpleTCP>(&archon_params);
    
    auto archon = std::unique_ptr<foxtrot::devices::archon> (
        new foxtrot::devices::archon(archontcp));
    
    auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&tpg_params);
    
    auto presgauge = std::unique_ptr<foxtrot::devices::TPG362> (new foxtrot::devices::TPG362(sport));
    
    auto modules = archon->getAllModules();
    
    
    //============== Archon Heater ====================//
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
    
    heater->setHeaterTarget(HeaterXHeaters::A, -100.);
    heater->setHeaterLimit(HeaterXHeaters::A, 25.);
    
    heater->setSource(1,gpio_source::clocked);
    heater->setLabel(1,"PCLAMP1");
    heater->setSource(2,gpio_source::clocked);
    heater->setLabel(1,"PCLAMP2");
    heater->setSource(3,gpio_source::clocked);
    heater->setLabel(1,"PCLAMP3");
    heater->setSource(4,gpio_source::clocked);
    heater->setLabel(1,"PCLAMP4");
    
    heater->setDirection(1,true);
    
    heater->setDIOPower(true);
    
    heater->apply();

    
    auto heaterptr = get_ptr_for_harness(heater);
    harness.AddDevice(std::move(heaterptr));
    //============Archon biases & drivers================//
    
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
     lvxbias->setEnable(true,i,true);
     lvxbias->setOrder(true,i,0);
    }
    
    lvxbias->setLimit(1,300);
    lvxbias->setLimit(2,300);
    lvxbias->setLimit(3,300);
    lvxbias->setLimit(4,300);
    
    lvxbias->setV(true,1,7.5);
    lvxbias->setV(true,2,-7.5);
    lvxbias->setV(true,3,7.5);
    lvxbias->setV(true,4,-7.5);
    
    
    auto lvxptr = get_ptr_for_harness(lvxbias);
    harness.AddDevice(std::move(lvxptr));
    
    auto xvbias = static_cast<foxtrot::devices::ArchonXV*>(&modules.at(1));
    xvbias->setLabel(false,1,"VBB");
    auto xvptr = get_ptr_for_harness(xvbias);
    harness.AddDevice(std::move(xvptr));
    
    auto clockdriver = static_cast<foxtrot::devices::ArchonDriver*>(&modules.at(9));
    clockdriver->setLabel(1,"IPHI1");
    clockdriver->setLabel(2,"IPHI2");
    clockdriver->setLabel(3,"IPHI3");
    clockdriver->setLabel(4,"IPHI4");
    clockdriver->setLabel(5,"RPHI1");
    clockdriver->setLabel(6,"RPHI2");
    clockdriver->setLabel(7,"RPHI3");
    clockdriver->setLabel(8,"PHIR");
    for(int i=1; i<=8; i++)
    {
     clockdriver->setEnable(i,true); 
    }
    
    auto cdptr = get_ptr_for_harness(clockdriver);
    clockdriver->setDeviceComment("CCD_clocks");
    harness.AddDevice(std::move(cdptr));
    
    auto clockdriver2 = static_cast<foxtrot::devices::ArchonDriver*>(&modules.at(2));
    auto cdptr2 = get_ptr_for_harness(clockdriver2);
    clockdriver2->setDeviceComment("spare_clocks");
    harness.AddDevice(std::move(cdptr2));
    
    //==============Archon A/Ds =====================//
    auto AD1 = static_cast<foxtrot::devices::ArchonAD*>(&modules.at(4));
    auto ad1ptr = get_ptr_for_harness(AD1);
    AD1->setDeviceComment("AD1");
    AD1->setPreampGain(false);
    harness.AddDevice(std::move(ad1ptr));
    
    auto AD2 = static_cast<foxtrot::devices::ArchonAD*>(&modules.at(5));
    auto ad2ptr = get_ptr_for_harness(AD2);
    AD2->setDeviceComment("AD2");
    AD2->setPreampGain(false);
    harness.AddDevice(std::move(ad2ptr));
    
    auto AD3 = static_cast<foxtrot::devices::ArchonAD*>(&modules.at(6));
    auto ad3ptr = get_ptr_for_harness(AD3);
    AD3->setDeviceComment("AD3");
    AD3->setPreampGain(false);
    harness.AddDevice(std::move(ad3ptr));
    
    auto AD4 = static_cast<foxtrot::devices::ArchonAD*>(&modules.at(7));
    auto ad4ptr = get_ptr_for_harness(AD4);
    AD4->setDeviceComment("AD4");
    AD4->setPreampGain(false);
    harness.AddDevice(std::move(ad4ptr));
    
    
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
    
    
//     archon->set_power(true);
    
    
    harness.AddDevice(std::move(presgauge));
    harness.AddDevice(std::move(archon));
    
    //===================radiometry system========================//
    
    auto spectrometer = std::unique_ptr<foxtrot::devices::stellarnet>(new foxtrot::devices::stellarnet("/home/dweatherill/Software/stellarnet/files_to_copy/stellarnet.hex",1000));
    harness.AddDevice(std::move(spectrometer));
    
    
    //setup power meter
    auto powermeterusb = std::make_shared<foxtrot::protocols::BulkUSB>(nullptr);
    auto powermeter = std::unique_ptr<foxtrot::devices::newport2936R>(new foxtrot::devices::newport2936R(powermeterusb));
    
    harness.AddDevice(std::move(powermeter));
    
    
    
    //====================illumination system========================//
    //setup monochromator
    auto cornerstone_serial = std::make_shared<foxtrot::protocols::SerialPort>(&cornerstone_params);
    auto monoch = std::unique_ptr<foxtrot::devices::cornerstone260>(new foxtrot::devices::cornerstone260(cornerstone_serial));
    
    //calibrate monochromator - TODO do from configuration file somewhere
    monoch->setGratingCalibration(1,600,0.99510,0.0872665,0.086534,"g1");
    monoch->setGratingCalibration(2,600,1.000800,3.22885911,0.085817,"g2");
    
    harness.AddDevice(std::move(monoch));


    auto scsiser = std::make_shared<foxtrot::protocols::scsiserial>(&psu_params);
    auto lamp_psu = std::unique_ptr<foxtrot::devices::Q250>(new foxtrot::devices::Q250(scsiser));
    
    harness.AddDevice(std::move(lamp_psu));
    
    
    
    return 0;  
};
}
