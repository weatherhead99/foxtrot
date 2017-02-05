#include "dummyDevice.h"
#include <rttr/registration>
#include <rttr/type>
#include <type_traits>


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
    return _distribution(_generator);
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
    return a1 + a2;
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
     );
}
