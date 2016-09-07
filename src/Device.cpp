#include "Device.h"


foxtrot::Device::Device(std::shared_ptr< foxtrot::CommunicationProtocol > proto)
: _proto(std::move(proto))
{

}
