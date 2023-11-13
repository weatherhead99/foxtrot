#include <filesystem>

#include <foxtrot/Logging.h>
#include <foxtrot/Error.h>

#include <foxtrot/ExperimentalSetup.h>
#include <foxtrot/DeviceHarness.h>

#include <foxtrot/parameter_defs.hh>

#include <boost/dll/smart_library.hpp>
#include <boost/dll/import_mangled.hpp>
#include <boost/dll/library_info.hpp>

using namespace foxtrot;

using boost::dll::experimental::smart_library;
using boost::system::system_error;
using foxtrot::DeviceHarness;
using foxtrot::mapofparametersets;
using std::make_unique;
using std::shared_ptr;
using std::string;
using std::filesystem::exists;

foxtrot::ft_plugin::ft_plugin(const string& file) 
: _fname(file), _lg("ft_plugin")
{
    
    _lg.strm(sl::debug) << "file: " << file;
	
    if(!exists(file))
    {
        _lg.Warning("file doesn't seem to exist...");
    }

    _lib = make_unique<smart_library>(file);

    boost::dll::library_info libinfo(file);
    
    for(auto& sym : libinfo.symbols())
    {
      if(sym.find("setup") != std::string::npos)
	_lg.strm(sl::debug) << "- " << sym;

      if(sym.find("test") != std::string::npos)
	_lg.strm(sl::debug) << "- " << sym;
      
    }

}

foxtrot::ft_plugin::~ft_plugin() = default;

void foxtrot::ft_plugin::reload()
{

  _lib = std::make_unique<smart_library>(_fname);

  if(!_lib->is_loaded())
    {
      _lg.strm(sl::error) << "library did not load...";
      throw std::runtime_error("shared library failed to load!");
    }


  boost::dll::library_info libinfo(_fname);
  _lg.strm(sl::debug) << "library symbols:" ;

  for(auto& sym : libinfo.symbols())
    {
      if(sym.find("setup") != std::string::npos)
	_lg.strm(sl::debug) << "- " << sym;

      if(sym.find("test") != std::string::npos)
	_lg.strm(sl::debug) << "- " << sym;
      
    }
  
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

    smart_library libb(_fname);


    try
      {
	setup_fun = libb.get_function<int(shared_ptr<foxtrot::DeviceHarness>, const mapofparametersets* const)>("setup");
	
      }
    catch(system_error& err)
      {
	_lg.strm(sl::warning) << "no setup function defined in library, trying legacy";
	_lg.strm(sl::debug) << "error caught was: " << err.what();
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
	    _lg.strm(sl::debug) << "error caught was: " << err.what();
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




