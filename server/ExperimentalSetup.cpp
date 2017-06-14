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


foxtrot::ExperimentalSetup::ExperimentalSetup(const std::string& setupfile, foxtrot::DeviceHarness& harness, const mapofparametersets* const paramsets )
: ft_plugin(setupfile), _harness(harness), _lg("ExperimentalSetup"), _paramsets(paramsets)
{
  
    auto setup_fun = get_function<int(*)(foxtrot::DeviceHarness&, const mapofparametersets* const)>("setup");
    if(!setup_fun)
    {
      throw std::runtime_error("no valid setup function found in setupfile...");
    }
    setup_fun(harness, _paramsets);
      
};


foxtrot::TelemetrySetup::TelemetrySetup(const std::string& file, foxtrot::TelemetryServer& telemserv, foxtrot::Client& cl)
: ft_plugin(file), _telemserv(telemserv), _lg("TelemetrySertup")
{
    auto fun = get_function<int(*)(foxtrot::TelemetryServer&, foxtrot::Client&)>("setup_telem");
    fun(_telemserv,cl);
}
