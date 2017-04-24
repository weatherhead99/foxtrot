#include "ExperimentalSetup.h"
#include "Error.h"

using namespace foxtrot;

foxtrot::ft_plugin::ft_plugin(const std::string& file)
{
    _dl = dlopen(file.c_str(), RTLD_LAZY);
  if(_dl == nullptr)
  {    
    throw std::runtime_error(dlerror());
  }
  
  //clear dlerror
  dlerror();
  
    
}

foxtrot::ft_plugin::~ft_plugin()
{
    dlclose(_dl);
}


foxtrot::ExperimentalSetup::ExperimentalSetup(const std::string& setupfile, foxtrot::DeviceHarness& harness)
: ft_plugin(setupfile), _harness(harness), _lg("ExperimentalSetup")
{
  
    auto setup_fun = get_function<int(*)(foxtrot::DeviceHarness&)>("setup");
    setup_fun(harness);
      
};


foxtrot::TelemetrySetup::TelemetrySetup(const std::string& file, foxtrot::TelemetryServer& telemserv)
: ft_plugin(file), _telemserv(telemserv), _lg("TelemetrySertup")
{
    auto fun = get_function<int(*)(foxtrot::TelemetryServer&)>("setup_telem");
    fun(_telemserv);
}
