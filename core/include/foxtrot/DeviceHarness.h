#pragma once
#include <memory>
#include <map>
#include <vector>
#include <climits>
#include <mutex>
#include <deque>
#include <variant>

#include <foxtrot/Device.h>
#include <foxtrot/Logging.h>

using std::shared_ptr;
using std::variant;
using std::unique_ptr;

using prop_or_meth = std::variant<rttr::property, rttr::method>;


namespace foxtrot
{
  using devptrunion = variant<shared_ptr<Device>, unique_ptr<Device>>;
    
  class DeviceHarness : public std::enable_shared_from_this<DeviceHarness>
    {
    public:
      [[nodiscard]] static std::shared_ptr<DeviceHarness> create();
      
      std::shared_ptr<DeviceHarness> ptr();
      //I think this was just for a pretty ancient compiler
      //int AddDevice(std::unique_ptr<Device,void(*)(Device*)> dev);
        int AddDevice(std::unique_ptr<Device> dev);
        int AddSharedDevice(std::shared_ptr<Device> dev);
        
        Device* const GetDevice(int id);
        
        void ClearDevices(unsigned contention_timeout_ms);
        void RemoveDevice(int id);
      
        
        
        [[ deprecated ]] std::vector<std::string> GetCapabilityNames(int devid);
      //devcapability GetDeviceCapability(int devid, const std::string& capname);
        
        //TODO:must be a more elegant way to do this trick
        const std::map<int,const Device*> GetDevMap() const;
	std::unique_lock<std::timed_mutex> lock_device_contentious(int devid, unsigned contention_timeout_ms);
	
	
    private:          
        DeviceHarness();      
        int _id = 0;
        foxtrot::Logging _lg;
        std::vector<devptrunion> _devvec;
      // std::vector<std::unique_ptr<Device,void(*)(Device*)>> _devvec;
        
        std::map<int, std::timed_mutex> _devmutexes;

       int _device_add_common(Device* dev);      
    };
    
    
    prop_or_meth getCapability(Device* dev, const std::string& capname);
    
    
    class propmethcallvisitor
    {
    public:
      propmethcallvisitor(DeviceHarness& harness, unsigned contention_timeout_ms);
      propmethcallvisitor(DeviceHarness& harness, rttr::variant arg, unsigned contention_timeout_ms);
      propmethcallvisitor(DeviceHarness& harness, std::vector<rttr::variant>& args, unsigned contention_timeout_ms);
      
      void operator()(rttr::method& meth);
      void operator()(rttr::property& prop);
      
    private:
      unsigned _timeout_ms;
      std::vector<rttr::variant>& _args;
      DeviceHarness& _harness;
      
      
    };
    
    
    
    

}
