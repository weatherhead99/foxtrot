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
using std::unique_ptr;

namespace foxtrot
{

  struct DeviceFactoryDescriptor
  { 
    string ident;
    string comment;
  };

  
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
    
    const vector<DeviceFactoryDescriptor>& get_available_devices() const
    { return _available_devices;}

    template<typename... Ts>
    unique_ptr<T> 
    open_device(const string& ident, Ts... args)
    {
      return std::make_unique<T>(std::forward<Ts...>(args...));
    };

    template<typename... Ts>
    int add_device(const string& ident, Ts... args)
    {
      auto uptr = std::make_unique<T>(std::forward<Ts...>(args...));

      auto strongptr = _harness.lock();
      if(strongptr)
	{
	  strongptr->AddDevice(std::move(uptr));
	  //TODO: once DeviceHarness adding returns devids, put it out here
	  return -1;
	}

      else {
	throw std::runtime_error("DeviceFactory no longer has access to DeviceHarness! Something is wrong");
      }
      
    };

  protected:
    std::vector<DeviceFactoryDescriptor> _available_devices;
  
  private:

    std::weak_ptr<DeviceHarness> _harness;
    foxtrot::Logging _lg;
  };


}
