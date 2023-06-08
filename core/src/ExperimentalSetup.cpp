#include <boost/filesystem.hpp>

#include <foxtrot/Logging.h>
#include <foxtrot/Error.h>

#include <foxtrot/ExperimentalSetup.h>
#include <foxtrot/DeviceHarness.h>

using namespace foxtrot;

foxtrot::ft_plugin::ft_plugin(const std::string& file) 
: _fname(file), _lg("ft_plugin")
{
    
    _lg.strm(sl::debug) << "file: " << file;
	
    if(!boost::filesystem::exists(file))
    {
        _lg.Warning("file doesn't seem to exist...");
    }
    
	#ifdef __linux__
	
		_dl = dlopen(file.c_str(), RTLD_LAZY);
		  
		//clear dlerror
	#else
        _dl = LoadLibrary(file.c_str());
	#endif
    
	  if(_dl == nullptr)
	  {    
#ifdef __linux__
          _lg.strm(sl::error) << "error loading library: " << dlerror();
		throw std::runtime_error(dlerror());
#else
        _lg.strm(sl::error) << "error loading library: " << GetLastError();
        throw std::runtime_error(std::to_string(GetLastError()));
#endif
	  }

#ifdef __linux__
	  dlerror();
#endif
}

foxtrot::ft_plugin::~ft_plugin()
{
    if(_dl != nullptr)
    {
		#ifdef __linux__
        dlclose(_dl);
		#else
        FreeLibrary(_dl);
		#endif
    }
}

void foxtrot::ft_plugin::reload()
{
    if(_dl != nullptr)
    {
		#ifdef __linux__
        dlclose(_dl);
		#else
			FreeLibrary(_dl);
		#endif
    }
	
	#ifdef __linux__
    _dl = dlopen(_fname.c_str(), RTLD_LAZY);
    
    dlerror();
	#else
        _dl = LoadLibrary(_fname.c_str());
	#endif
    
};


foxtrot::ExperimentalSetup::ExperimentalSetup(const std::string& setupfile, foxtrot::DeviceHarness& harness, const foxtrot::mapofparametersets* const paramsets )
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




