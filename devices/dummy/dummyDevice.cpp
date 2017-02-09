#include "dummyDevice.h"
#include <rttr/registration>
#include <rttr/type>
#include <type_traits>
#include <iostream>
#include "DeviceError.h"

// dummyDevice::dummyDevice() : Device(nullptr)
foxtrot::devices::dummyDevice::dummyDevice() : Device(nullptr)
{
}

// int dummyDevice::getCounter()
int foxtrot::devices::dummyDevice::getCounter()
{
    return _counter++;
}


// double dummyDevice::getRandomDouble() 
double foxtrot::devices::dummyDevice::getRandomDouble()
{
    std::cout << "generating random double.." << std::endl;
    auto num  = _distribution(_generator);
    std::cout << "num: " << num << std::endl;
    return num;
}

// void dummyDevice::resetCounter()
void foxtrot::devices::dummyDevice::resetCounter()
{
    _counter = 0;
}


const std::string foxtrot::devices::dummyDevice::getDeviceTypeName() const
{
    return "dummyDevice";
}

int foxtrot::devices::dummyDevice::add(int a1, int a2)
{
    std::cout << "adding numbers: " << a1 << " and " << a2 << std::endl;
    return a1 + a2;
}

void foxtrot::devices::dummyDevice::brokenMethod()
{
    class DeviceError except("womble!");
    throw except;
}


std::vector<int> foxtrot::devices::dummyDevice::unsupportedtype()
{
    std::vector<int> out {1,2,3,4};
    return out;
}



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::dummyDevice;
 registration::class_<dummyDevice>("foxtrot::dummyDevice")
 .property_readonly("getCounter", &dummyDevice::getCounter)
 .property_readonly("getRandomDouble", &dummyDevice::getRandomDouble)
 .method("resetCounter", &dummyDevice::resetCounter)
 .method("add", &dummyDevice::add)
 (
     parameter_names("a1","a2")
     )
 .method("brokenMethod",&dummyDevice::brokenMethod)
 .property_readonly("unsupportedtype",&dummyDevice::unsupportedtype)
 ;
}
