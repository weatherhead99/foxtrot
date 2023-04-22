#pragma once
#include <rttr/type>
#include <string>

#include "Device.h"
#include "Logging.h"
#include "DeviceHarness.h"
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::shared_ptr;


namespace foxtrot
{
  template<typename T>
  class DeviceFactory: public Device
  {
    RTTR_ENABLE(Device)
    using DeviceType = T;
    
  public:
    DeviceFactory(shared_ptr<DeviceHarness> harness,
		  const string& comment = "",
		  shared_ptr<CommunicationProtocol> proto=nullptr): _harness(harness), _lg("DeviceFactory"), Device(proto, comment) {};
    

    virtual void discover() = 0;
    const vector<string>& get_available_devices() const
    { return _available_devices;}

    template<typename... Ts>
    shared_ptr<T> 
    open_device(const string& ident, Ts... args)
    {
      throw std::logic_error("don't know how to make a device with this set of arguments");
    };

    virtual shared_ptr<T> open_device(const string& ident) = 0;

    virtual shared_ptr<T> open_device(const string& ident, const std::vector<rttr::variant>& args) = 0;
  
  private:
    std::vector<string> _available_devices;
    std::weak_ptr<DeviceHarness> _harness;
    foxtrot::Logging _lg;
  };


}
