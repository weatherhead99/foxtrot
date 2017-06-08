#include "devices/dummy/dummyDevice.h"
#include "DeviceHarness.h"
#include <memory>
#include "DeviceError.h"

#include <iostream>
#include "Logging.h"

using std::cout;
using std::endl;

using namespace foxtrot;

extern "C" {
int setup(foxtrot::DeviceHarness& harness)
{
    
  foxtrot::Logging lg("dummy_setup");
    
  lg.Info("in setup function...");
    
  lg.Debug("setting up dummy device..");
  
  auto devfun = [] (foxtrot::Device* dev) { delete dev;};
  
  
    auto devptr = std::unique_ptr<foxtrot::devices::dummyDevice,void(*)(Device*)>(
        new foxtrot::devices::dummyDevice,devfun);
    
    lg.Debug("adding to harness..");
    harness.AddDevice(std::move(devptr));
    
    
    return 0;
};
}
