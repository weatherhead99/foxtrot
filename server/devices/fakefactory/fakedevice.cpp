#include "fakedevice.hh"
#include <string>
#include <random>
#include <sstream>

using namespace foxtrot;
using namespace foxtrot::devices;
using std::string;


namespace foxtrot
{
  namespace devices
  {
  class fakeDeviceFactoryImpl {
  public:
    string _value;

    fakeDeviceFactoryImpl(std::size_t seed) : _dist(0.0, 1.0) { _eng.seed(seed); }

    std::default_random_engine _eng;
    std::uniform_real_distribution<double> _dist;
  };
  }
}

fakeDeviceFactory::fakeDeviceFactory(shared_ptr<DeviceHarness> harness,
                                              int n_devices_avail,
                                              const string &comment)
  : DeviceFactory(harness, comment), _n_devices_avail(n_devices_avail) {}

void fakeDeviceFactory::discover(){

  std::ostringstream oss;
  for(int i=0 ; i < _n_devices_avail; i++)
    {
      oss <<"fake" << std::to_string(i);
      _available_devices.push_back({oss.str(), "fake device"});
      oss.str("");
				 
    }


  };


fakeDevice::fakeDevice(const string &my_ident, std::size_t rndseed)
  : _impl(new fakeDeviceFactoryImpl(rndseed)), Device(nullptr, my_ident)
{
  _impl->_value = my_ident;
}

double fakeDevice::getRandomDouble()
{
  return _impl->_dist(_impl->_eng);
}







RTTR_REGISTRATION {
  using namespace rttr;


  registration::class_<fakeDevice>("foxtrot::devices::fakeDevice")
    .property_readonly("getRandomDouble", &fakeDevice::getRandomDouble);
  
  registration::class_<fakeDeviceFactory>("foxtrot::devices::fakeDeviceFactory");

}


