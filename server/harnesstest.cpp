#include "dummyDevice.h"
#include <iostream>
#include <memory>
#include "DeviceHarness.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    
    std::unique_ptr<foxtrot::devices::dummyDevice> dev(new foxtrot::devices::dummyDevice);
    
    foxtrot::DeviceHarness harness;
    
    harness.AddDevice(std::move(dev));
    
    


}
