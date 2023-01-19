#pragma once
#include <string>
#ifdef linux
#include <dlfcn.h>
#else
#define NOMINMAX
#include <windows.h>
#endif

#include <variant>
#include <map>

#include <foxtrot/Logging.h>
#include <foxtrot/StubError.h>


// note: the below are redefinitions of stuff that's in protocols. One day
// all parameter handling should probably be moved into core

namespace foxtrot
{

    using parameterset = std::map<std::string, std::variant<int, std::string>>;
    using mapofparametersets = std::map<std::string, foxtrot::parameterset>;

  
    class DeviceHarness;
    class TelemetryServer;
    class Client;
    
    class ft_plugin
    {
    public:
        ft_plugin(const std::string& file);
        ~ft_plugin();
        
        void reload();
        
    protected:
        
        template<typename funtp> funtp get_function(const std::string& name)
        {
		#ifdef linux
            _lg.Trace("calling dlsym...");
            auto sym = dlsym(_dl,name.c_str());
        #else
            _lg.Trace("calling GetProcAddress...");
            auto sym = GetProcAddress(_dl,name.c_str());
		#endif
            
          if(sym == nullptr)
          {
              _lg.Error("couldn't find matching function " + name + " in plugin file");
              throw std::runtime_error("couldn't find matching function " + name + " in plugin file");
          }
          
          auto fun = reinterpret_cast<funtp>(sym);
          return fun;
        };
        
        Logging _lg;
    private:
#ifdef linux
        void* _dl = nullptr;
#else
        HMODULE _dl = nullptr;
#endif
        std::string _fname;
    };
    
    
    class ExperimentalSetup : public ft_plugin
    {
    public:
        ExperimentalSetup(const std::string& setupfile, DeviceHarness& harness, const mapofparametersets* const paramsets = nullptr);
        void reset();
        
    private:
        DeviceHarness& _harness;
	const mapofparametersets* const _paramsets;
     int(*setup_fun)(foxtrot::DeviceHarness&, const mapofparametersets* const) = nullptr;
    };
    
    class TelemetrySetup : public ft_plugin
    {
    public:
        TelemetrySetup(const std::string& file,  TelemetryServer& telemserv, Client& cl);
        
    private:
        
        TelemetryServer& _telemserv;
          
    };


}
