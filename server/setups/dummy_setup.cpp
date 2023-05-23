#include <memory>
#include <map>
#include <iostream>
#include <sstream>

#include <boost/variant.hpp>

#include <foxtrot/devices/dummyDevice.h>
#include <foxtrot/devices/fakedevice.hh>
#include <foxtrot/DeviceHarness.h>
#include <foxtrot/DeviceError.h>

#include <foxtrot/Logging.h>
#include "dummy_setup.h"

using std::cout;
using std::endl;

using namespace foxtrot;

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


    lg.Info("attempting to set up fakeDeviceFactory");

    using foxtrot::devices::fakeDeviceFactory;

    //    auto fakedevfactptr = std::make_unique<fakeDeviceFactory>(harness, 3,
    //							      "fakeDeviceFactory");

    //harness.AddDevice(std::move(fakedevfactptr));
    
    return 0;
};
}
