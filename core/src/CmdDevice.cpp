#include <foxtrot/CmdDevice.h>
#include <foxtrot/Logging.h>

foxtrot::CmdDevice::CmdDevice(std::shared_ptr< foxtrot::CommunicationProtocol > proto): Device(proto)
{

}

foxtrot::CmdDevice::CmdDevice(std::shared_ptr< CommunicationProtocol > proto, const std::string& comment)
: Device(proto, comment)
{
}

void foxtrot::CmdDevice::cmd_no_response(const std::string& request)
{
  foxtrot::Logging lg("CmdDevice");
  lg.strm(sl::warning) << "cmd_no_response called base implementation that does nothing! Should be overridden";
  
};
