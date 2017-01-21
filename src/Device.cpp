#include "Device.h"
#include <rttr/registration>


foxtrot::Device::Device(std::shared_ptr< foxtrot::CommunicationProtocol > proto)
: _proto(std::move(proto))
{

}


const std::string foxtrot::Device::getDeviceTypeName() const
{
    return "Device";
}



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::Device;
 registration::class_<Device>("foxtrot::Device")
 .method("getDeviceTypeName", &Device::getDeviceTypeName)
 ;
       
}





