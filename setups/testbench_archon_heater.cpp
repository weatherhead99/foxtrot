#include "DeviceHarness.h"
#include "devices/archon/archon.h"
#include "devices/archon/archon_modules.h"
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

#include "testbench_setup_funcs.h"

using mapofparametersets = std::map<std::string, foxtrot::parameterset>;




template<typename T> void setup_with_disable(const std::string& device_name, std::map<std::string,parameter>& params,
					     foxtrot::Logging& lg, T fun)
{
  
  auto disable_str = "disable_" + device_name;
  if(!boost::get<int>(params[disable_str]))
  {
    fun();
  }
  else
  {
    lg.strm(sl::warning) << "device: " << device_name << " was disabled in the config file...";
  }
  
};




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
    
    auto tpg_params = params->at("tpg_params");
    
    setup_with_disable("TPG362", setup_params, lg,
		       [&harness, &tpg_params, &lg] () {
				lg.Info("setting up TPG pressure gauge...");
				auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&tpg_params);
    
				auto presgauge = std::unique_ptr<foxtrot::devices::TPG362> (new foxtrot::devices::TPG362(sport));
				harness.AddDevice(std::move(presgauge));});
  
    
    setup_with_disable("archon",setup_params,lg,
		     [&harness, &setup_params, &params, &lg]()
		     {
    
			auto archon_reset = static_cast<bool>(boost::get<int>(setup_params.at("archon_reset")));
			lg.Info("setting up Archon....");
			auto archon_params = params->at("archon_params");
			auto archontcp = std::make_shared<foxtrot::protocols::simpleTCP>(&archon_params);
			
			auto archon = std::unique_ptr<foxtrot::devices::archonraw> (
			    new foxtrot::devices::archonraw(archontcp));
			
			archon->settrigoutpower(true);
			auto modules = archon->getAllModules();
			setup_heaterX(modules, harness, lg);
			
			//============Archon biases & drivers================//
			setup_hvxbias(modules,harness,lg);
			setup_lvxbias(modules,harness,lg, archon_reset);
			setup_xvbias(modules,harness,lg, archon_reset);
			setup_clockdriver(modules,harness,lg, archon_reset);
			setup_clockdriver2(modules,harness,lg, archon_reset);
			
			//==============Archon A/Ds =====================//
			
			setup_ADCs(modules,harness,lg, archon_reset);
    
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
		     });
    
        
    //===================radiometry system========================//
    
    setup_with_disable("stellarnet", setup_params, lg,
		       [&harness, &lg, &setup_params] ()
		       {
			 lg.Info("setting up Stellarnet Spectrometer");
			 auto firmware_file = boost::get<std::string>(setup_params.at("stellarnet_firmware"));

			 auto spectrometer = std::unique_ptr<foxtrot::devices::stellarnet>(new foxtrot::devices::stellarnet(firmware_file,1000));
			 harness.AddDevice(std::move(spectrometer));
		       });
      
    
    
    setup_with_disable("powermeter", setup_params, lg,
		       [&harness, &lg, &params] ()
		       {
			  lg.Info("setting up Newport 2936R power meter...");
			  auto newport_params = params->at("newport_params_serial");
			  auto powermeterserial = std::make_shared<foxtrot::protocols::SerialPort>(&newport_params);
			  
		      //     auto powermeterusb = std::make_shared<foxtrot::protocols::BulkUSB>(&newport_params);
			  auto powermeter = std::unique_ptr<foxtrot::devices::newport2936R>(new foxtrot::devices::newport2936R(powermeterserial));
			  
			  powermeter->setChannel(1);
			  #ifndef NEW_RTTR_API
			  powermeter->setMode(foxtrot::devices::powermodes::Integrate);
			  #else
			  powermeter->setMode(2);
			  #endif
			  powermeter->setExternalTriggerMode(1);
			  powermeter->setTriggerEdge(1);
			  
			  harness.AddDevice(std::move(powermeter));
		       });
    
    
    
    //====================illumination system========================//
    //setup monochromator
    
    setup_with_disable("monochromator",setup_params,lg,
		       [&harness, &lg, &params]() {
    
			  lg.Info("setting up Cornerstone 260 monochromator");
			  auto cornerstone_params = params->at("cornerstone_params");
			  auto cornerstone_serial = std::make_shared<foxtrot::protocols::SerialPort>(&cornerstone_params);
			  auto monoch = std::unique_ptr<foxtrot::devices::cornerstone260>(new foxtrot::devices::cornerstone260(cornerstone_serial));
			  
			  //calibrate monochromator - TODO do from configuration file somewhere
			  monoch->setGratingCalibration(1,600,0.99510,0.0872665,0.086534,"g1");
			  monoch->setGratingCalibration(2,600,1.000800,3.22885911,0.085817,"g2");
			  monoch->setGratingCalibration(3,600,0.0000,3.22885911,0.000,"");
			  harness.AddDevice(std::move(monoch));});

    setup_with_disable("Q250", setup_params, lg,
	[&harness, &lg, &params] () { 
	  lg.Info("setting up Newport Q250 Power Supply");
	  auto psu_params = params->at("psu_params");
	  auto scsiser = std::make_shared<foxtrot::protocols::scsiserial>(&psu_params);
	  auto lamp_psu = std::unique_ptr<foxtrot::devices::Q250>(new foxtrot::devices::Q250(scsiser));
	  harness.AddDevice(std::move(lamp_psu));
	});

    
    return 0;  
};
}
