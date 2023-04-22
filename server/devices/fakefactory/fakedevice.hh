#pragma once

#include <foxtrot/Device.h>
#include <foxtrot/DeviceFactory.hh>
#include <rttr/registration>


namespace foxtrot
{
  namespace devices
  {
    class fakeDeviceImpl;
    
    class fakeDevice : public Device
    {
      RTTR_ENABLE(Device)

      public:
      double getRandomDouble();
      void setValue(const string& val);
      string getValue() const;
      
      private:
      fakeDevice(const string& my_ident, std::size_t rndseed);
      fakeDeviceImpl* _impl = nullptr;
      
    };

    class fakeDeviceFactory : public DeviceFactory<fakeDevice>
    {

      friend class fakeDevice;
      RTTR_ENABLE(DeviceFactory<fakeDevice>)

      public:
      fakeDeviceFactory(shared_ptr<DeviceHarness> harness,
			int n_devices_avail,
			const string& comment = ""
			);

      void discover() override;
      shared_ptr<Device> open_device(const string& ident,
				     const std::vector<rttr::variant>& args) override;

    private:
      int _n_devices_avail;
      
    };


  }

}
