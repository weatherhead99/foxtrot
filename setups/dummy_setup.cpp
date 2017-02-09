#include "devices/dummy/dummyDevice.h"
#include "DeviceHarness.h"
#include <memory>

#include <iostream>

using std::cout;
using std::endl;

extern "C" {
int setup(foxtrot::DeviceHarness& harness)
{
    
    std::cout << "in setup function..." << std::endl;
    
    auto devptr = std::unique_ptr<foxtrot::devices::dummyDevice>(
        new foxtrot::devices::dummyDevice);
    
    
    harness.AddDevice(std::move(devptr));
    
    
    return 0;
};
}
