#include "dummyDevice.h"
#include <rttr/registration>
#include <rttr/type>

#include <type_traits>


foxtrot::devices::dummyDevice::dummyDevice() : Device(nullptr)
// dummyDevice::dummyDevice() : Device(nullptr)
{
}

int foxtrot::devices::dummyDevice::getCounter()
// int dummyDevice::getCounter()
{
    return _counter++;
}


double foxtrot::devices::dummyDevice::getRandomDouble()
// double dummyDevice::getRandomDouble() 
{
    return _distribution(_generator);
}

void foxtrot::devices::dummyDevice::resetCounter()
// void dummyDevice::resetCounter()
{
    _counter = 0;
}


const std::string foxtrot::devices::dummyDevice::getDeviceTypeName() const
{
    return "dummyDevice";
}


RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::devices::dummyDevice;
 registration::class_<dummyDevice>("foxtrot::dummyDevice")
 .property_readonly("getCounter", &dummyDevice::getCounter)
 .property_readonly("getRandomDouble", &dummyDevice::getRandomDouble)
 .method("resetCounter", &dummyDevice::resetCounter)

 ;
}
