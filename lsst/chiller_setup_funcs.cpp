#include "chiller_setup_funcs.hh"
#include <foxtrot/DeviceHarness.h>
#include <foxtrot/protocols/SerialPort.h>

#ifdef MAGIS_CHILLER
#include <foxtrot/ft_devices_exports/ssc_tcube_edge.hh>


#endif



void setup_chiller(foxtrot::Logging& lg,
		   foxtrot::DeviceHarness& harness,
		   const mapofparametersets* const params)

{
#ifndef MAGIS_CHILLER
  lg.strm(sl::info) << "MAGIS chiller support not compiled in, not setting it up";

#else

  std::shared_ptr<foxtrot::devices::SerialPort> sport = nullptr;
  
  try
    {
      auto chiller_params = params->at("chiller_params");

      if(std::get<int>(chiller_params["disable_chiller"]))
	{
	  lg.strm(sl::info) << "chiller disabled in the config file, not continuing setup...";
	  return;
	}
      sport = std::make_shared<foxtrot::devices::SerialPort>(&chiller_params);
    }
  catch(std::out_of_range)
    {
      lg.strm(sl::warning) << "chiller parameters not found, not enabling" ;
      return;
    }

  auto chiller = std::make_unique<foxtrot::devices::TCubeEdge>(sport);

  chiller->setDeviceComment("SSC_chiller");
  harness.AddDevice(std::move(chiller));
  
     
#endif
  

  

};

  
