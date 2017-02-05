
#include "ServerImpl.h"
#include "DeviceHarness.h"
#include "dummy/dummyDevice.h"
#include <memory>
#include <iostream>
using namespace foxtrot;

int main(int argc, char** argv)
{
 
 
    std::unique_ptr<Device> dev(new devices::dummyDevice);
    dev->setDeviceComment("test dummy device");
    
    DeviceHarness harness;
    
    harness.AddDevice(std::move(dev));
    
    
    foxtrot::ServerImpl serv("test server",harness);
    serv.Run();
    


};
