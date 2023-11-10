#include <filesystem>

#include <foxtrot/Logging.h>
#include <foxtrot/Error.h>

#include <foxtrot/ExperimentalSetup.h>
#include <foxtrot/DeviceHarness.h>

#include <boost/dll/smart_library.hpp>
#include <boost/dll/import_mangled.hpp>

using namespace foxtrot;

using boost::dll::experimental::smart_library;
using std::shared_ptr;
using foxtrot::DeviceHarness;
using std::filesystem::exists;
using std::make_unique;
using std::string;
using foxtrot::mapofparametersets;
using boost::system::system_error;

foxtrot::ft_plugin::ft_plugin(const string& file) 
: _fname(file), _lg("ft_plugin")
{
    
    _lg.strm(sl::debug) << "file: " << file;
	
    if(exists(file))
    {
        _lg.Warning("file doesn't seem to exist...");
    }

    _lib = make_unique<smart_library>(file);

}

foxtrot::ft_plugin::~ft_plugin() = default;

void foxtrot::ft_plugin::reload()
{

  _lib = std::make_unique<smart_library>(_fname);

}


foxtrot::ExperimentalSetup::ExperimentalSetup(
    const string &setupfile,
    shared_ptr<DeviceHarness> harness,
    const mapofparametersets *const paramsets) : ft_plugin(setupfile), _harness(harness), _paramsets(paramsets)
{
    _lg.setLogChannel("ExperimentalSetup");
    
    reset();
}
    


void foxtrot::ExperimentalSetup::reset()
{
    _lg.Debug("clearing all devices...");
    _harness->ClearDevices(1000);
    
    _lg.Debug("reloading setup file...");
    reload();


    try
      {
	setup_fun = _lib->get_function<int(shared_ptr<DeviceHarness>, const mapofparametersets* const)>("setup");
      }
    catch(system_error& err)
      {
	_lg.strm(sl::warning) << "no setup function defined in library, trying legacy";
	use_legacy = true;

      }


    if(use_legacy)
      {
	bool use_c_linkage = false;
	try
	  {
	    setup_fun_legacy = _lib->get_function<int(foxtrot::DeviceHarness&, const mapofparametersets* const)>("setup");
	  }
	catch(system_error& err)
	  {
	    _lg.strm(sl::warning) << "no setup function with legacy signature detected, trying C function";
	    use_c_linkage = true;
	  }

	if(use_c_linkage)
	  {
	    try
	      {
		auto lib2 = _lib->shared_lib();
		setup_fun_legacy = lib2.get<int(foxtrot::DeviceHarness&, const mapofparametersets* const)>("setup");
	      }
	    catch(system_error& err)
	      {
		_lg.strm(sl::error) << "all function loadings failed...";
		throw std::runtime_error("no valid setup function found in setupfile...");
		
	      }
		
	  }
      }
  
    
    _lg.Debug("running setup function...");

    if(use_legacy)
      setup_fun_legacy(*_harness,_paramsets);
    else
      setup_fun(_harness, _paramsets);
    
}



foxtrot::TelemetrySetup::TelemetrySetup(const std::string& file, foxtrot::TelemetryServer& telemserv, foxtrot::Client& cl)
: ft_plugin(file), _telemserv(telemserv)
{
  //    _lg.setLogChannel("TelemetrySetup");
  //  auto fun = get_function<int(*)(foxtrot::TelemetryServer&, foxtrot::Client&)>("setup_telem");
  //  fun(_telemserv,cl);
}




