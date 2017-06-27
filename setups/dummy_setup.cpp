#include "devices/dummy/dummyDevice.h"
#include "DeviceHarness.h"
#include <memory>
#include "DeviceError.h"

#include <iostream>
#include "Logging.h"
#include <map>
#include <boost/variant.hpp>
#include <sstream>

using std::cout;
using std::endl;

using namespace foxtrot;
using mapofparametersets = std::map<std::string, std::map<std::string, boost::variant<unsigned,int,std::string>>>;

extern "C" {
int setup(foxtrot::DeviceHarness& harness, const mapofparametersets* const paramsets)
{
  
    
  foxtrot::Logging lg("dummy_setup");
  
  if(!paramsets)
  {
    lg.Warning("no parameter sets supplied");
  }
  else
  {
    std::ostringstream oss;
    for(auto& paramset : *paramsets)
    {
      oss << paramset.first << ", ";
    }
    
   lg.Info("parameter sets received: "  + oss.str());
    
  }
  
  
    
  lg.Info("new function");
  lg.Info("in setup function...");
    
  lg.Debug("setting up dummy device..");
  
  auto devfun = [] (foxtrot::Device* dev) { delete dev;};
  
  
    auto devptr = std::unique_ptr<foxtrot::devices::dummyDevice,void(*)(Device*)>(
        new foxtrot::devices::dummyDevice("dummy1"),devfun);
    
    auto devptr2 = std::unique_ptr<foxtrot::devices::dummyDevice,void(*)(Device*)>(
	new foxtrot::devices::dummyDevice("dummy2"),devfun);
    
    
    lg.Debug("adding to harness..");
    harness.AddDevice(std::move(devptr));
    harness.AddDevice(std::move(devptr2));
    
    
    return 0;
};
}
