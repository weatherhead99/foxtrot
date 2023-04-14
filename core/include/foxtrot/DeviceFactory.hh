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
  class DeviceFactory : public Device
  {
    
  public:
    DeviceFactory(shared_ptr<DeviceHarness> harness,
		  const string& comment = "",
		  shared_ptr<CommunicationProtocol> proto=nullptr);
    
    RTTR_ENABLE(Device)
    virtual void discover() = 0;
    const vector<string>& get_available_devices() const;

    template<typename... Ts>
    shared_ptr<Device> 
    open_device(const string& ident, Ts... args)
    {
      throw std::logic_error("don't know how to make a device with this set of arguments");
    };

    virtual shared_ptr<Device> open_device(const string& ident) = 0;

    virtual shared_ptr<Device> open_device(const string& ident, const std::vector<rttr::variant>& args) = 0;
  
  private:
    std::vector<string> _available_devices;
    std::weak_ptr<DeviceHarness> _harness;
    foxtrot::Logging _lg;
  };


}
