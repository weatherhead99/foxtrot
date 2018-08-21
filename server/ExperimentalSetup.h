#pragma once
#include <string>
#include "Logging.h"
#ifdef linux
#include <dlfcn.h>
#endif
#include <boost/variant.hpp>
#include "CommunicationProtocol.h"
#include "StubError.h"



using mapofparametersets = std::map<std::string, foxtrot::parameterset>;

namespace foxtrot
{
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
          auto sym = dlsym(_dl,name.c_str());
			
          if(sym == nullptr)
          {
              throw std::runtime_error("couldn't find matching function " + name + " in plugin file");
          }
          
          auto fun = reinterpret_cast<funtp>(sym);
          return fun;
        #else
			throw StubError("not implemented on windows yet...");
		#endif
        };
        
        Logging _lg;
    private:
        void* _dl = nullptr;
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
