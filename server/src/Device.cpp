#include <rttr/registration>

#include <foxtrot/server/Device.h>


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
 .property_readonly("getDeviceTypeName", &Device::getDeviceTypeName)
 .property_readonly("getDeviceComment", &Device::getDeviceComment)
 ;
       
}





