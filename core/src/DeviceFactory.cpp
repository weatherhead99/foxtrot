#include <foxtrot/DeviceFactory.hh>

using namespace foxtrot;
using std::string;

DeviceFactory::DeviceFactory(shared_ptr<DeviceHarness> harness,
			     const string& comment,
			     shared_ptr<CommunicationProtocol> proto)
  : _harness(harness), _lg("DeviceFactory"), Device(proto, comment){};

const vector<string>& DeviceFactory::get_available_devices() const
{
  return _available_devices;
}




