#include "fakedevice.hh"
#include <string>
#include <random>

using namespace foxtrot;
using namespace foxtrot::devices;
using std::string;

class foxtrot::devices::fakeDeviceFactoryImpl
{
  string _value;
  std::default_random_engine eng;
  

};




fakeDeviceFactory::fakeDeviceFactory(shared_ptr<DeviceHarness> harness,
                                              int n_devices_avail,
                                              const string &comment)
  : DeviceFactory(harness, comment), _n_devices_avail(n_devices_avail), _impl(new fakeDeviceFactoryImpl) {}


fakeDeviceFactory::~fakeDeviceFactory()
{

}



