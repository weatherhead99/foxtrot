#include "Device.h"
#include <rttr/registration>


foxtrot::Device::Device(std::shared_ptr< foxtrot::CommunicationProtocol > proto, const std::string& comment)
: _proto(std::move(proto)), _devcomment(comment)
{

}


const std::string foxtrot::Device::getDeviceTypeName() const
{
    return "Device";
}


const std::string foxtrot::Device::getDeviceComment() const
{
    return _devcomment;
};

void foxtrot::Device::setDeviceComment(const std::string& comment)
{
    _devcomment = comment;
}



RTTR_REGISTRATION
{
 using namespace rttr;
 using foxtrot::Device;
 registration::class_<Device>("foxtrot::Device")
 .method("getDeviceTypeName", &Device::getDeviceTypeName)
 ;
       
}





