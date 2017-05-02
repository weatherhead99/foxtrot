#pragma once
#include <string>
#include "Logging.h"
#include <dlfcn.h>

namespace foxtrot
{
    class DeviceHarness;
    class TelemetryServer;
    
    class ft_plugin
    {
    public:
        ft_plugin(const std::string& file);
        ~ft_plugin();
        
    protected:
        
        template<typename funtp> funtp get_function(const std::string& name)
        {
          auto sym = dlsym(_dl,name.c_str());
          if(sym == nullptr)
          {
              throw std::runtime_error("couldn't find matching function " + name + " in plugin file");
          }
          
          auto fun = reinterpret_cast<funtp>(sym);
          return fun;
            
        };
        
    private:
        void* _dl;
    };
    
    
    class ExperimentalSetup : public ft_plugin
    {
    public:
        ExperimentalSetup(const std::string& setupfile, DeviceHarness& harness);
        
    private:
       Logging _lg;
        DeviceHarness& _harness;
    };
    
    class TelemetrySetup : public ft_plugin
    {
    public:
        TelemetrySetup(const std::string& file,  TelemetryServer& telemserv);
        
    private:
        Logging _lg;
        TelemetryServer& _telemserv;
          
    };


}
