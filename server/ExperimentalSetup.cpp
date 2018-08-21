#include "ExperimentalSetup.h"
#include "Error.h"
#include "DeviceHarness.h"
#include <Logging.h>

using namespace foxtrot;

foxtrot::ft_plugin::ft_plugin(const std::string& file) 
: _fname(file), _lg("ft_plugin")
{
    
    _lg.strm(sl::debug) << "file: " << file;
	
	#ifdef linux
	
		_dl = dlopen(file.c_str(), RTLD_LAZY);
	  if(_dl == nullptr)
	  {    
		throw std::runtime_error(dlerror());
	  }
		  
		//clear dlerror
		dlerror();
	#else
		throw foxtrot::StubError("not implemented on Windows yet..");
	#endif
    
}

foxtrot::ft_plugin::~ft_plugin()
{
    if(_dl != nullptr)
    {
		#ifdef linux
        dlclose(_dl);
		#else
			_lg.Error("FIXME: not implemented on windows yet");
		#endif
    }
}

void foxtrot::ft_plugin::reload()
{
    if(_dl != nullptr)
    {
        dlclose(_dl);
    }
    
    _dl = dlopen(_fname.c_str(), RTLD_LAZY);
    
    dlerror();
    
};


foxtrot::ExperimentalSetup::ExperimentalSetup(const std::string& setupfile, foxtrot::DeviceHarness& harness, const mapofparametersets* const paramsets )
: ft_plugin(setupfile), _harness(harness),  _paramsets(paramsets)
{
    _lg.setLogChannel("ExperimentalSetup");
    setup_fun = get_function<int(*)(foxtrot::DeviceHarness&, const mapofparametersets* const)>("setup");
    if(!setup_fun)
    {
      throw std::runtime_error("no valid setup function found in setupfile...");
    }
    setup_fun(harness, _paramsets);
      
};

void foxtrot::ExperimentalSetup::reset()
{
    _lg.Debug("clearing all devices...");
    _harness.ClearDevices(1000);
    
    _lg.Debug("reloading setup file...");
    reload();
    
    
    if(setup_fun == nullptr)
    {
        throw std::runtime_error("setup function handle is stale!");
    }
    
    _lg.Debug("running setup function...");
    
    setup_fun(_harness,_paramsets);
    
    
}



foxtrot::TelemetrySetup::TelemetrySetup(const std::string& file, foxtrot::TelemetryServer& telemserv, foxtrot::Client& cl)
: ft_plugin(file), _telemserv(telemserv)
{
    _lg.setLogChannel("TelemetrySetup");
    auto fun = get_function<int(*)(foxtrot::TelemetryServer&, foxtrot::Client&)>("setup_telem");
    fun(_telemserv,cl);
}




