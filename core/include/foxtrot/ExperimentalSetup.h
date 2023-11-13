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
#include <memory>

#include <foxtrot/Logging.h>
#include <foxtrot/StubError.h>

#include <foxtrot/parameter_defs.hh>


// note: the below are redefinitions of stuff that's in protocols. One day
// all parameter handling should probably be moved into core
namespace boost
{
  namespace dll
  {
    namespace experimental
    {

      class smart_library;
    }
  }
}


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

        Logging _lg;
        std::string _fname;
      std::unique_ptr<boost::dll::experimental::smart_library> _lib;


    };

  using foxtrot::mapofparametersets;
    
    class ExperimentalSetup : public ft_plugin
    {
    public:
      ExperimentalSetup(const std::string& setupfile, std::shared_ptr<DeviceHarness> harness,
			const foxtrot::mapofparametersets* const paramsets = nullptr);
        void reset();
        
    private:
      void call_setup_fun();
      
      std::shared_ptr<DeviceHarness> _harness;
      const mapofparametersets* const _paramsets;
      std::function<int(foxtrot::DeviceHarness&, const mapofparametersets* const)> setup_fun_legacy;
      std::function<int(std::shared_ptr<foxtrot::DeviceHarness>, const mapofparametersets* const)> setup_fun;
      bool use_legacy = false;
    };
    
  class TelemetrySetup : public ft_plugin
    {
    public:
     [[deprecated]] TelemetrySetup(const std::string& file,  TelemetryServer& telemserv, Client& cl);
        
    private:
        
        TelemetryServer& _telemserv;
          
    };


}
