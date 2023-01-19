#include <foxtrot/CmdDevice.h>

foxtrot::CmdDevice::CmdDevice(std::shared_ptr< foxtrot::CommunicationProtocol > proto): Device(proto)
{

}

foxtrot::CmdDevice::CmdDevice(std::shared_ptr< CommunicationProtocol > proto, const std::string& comment)
: Device(proto, comment)
{
}
