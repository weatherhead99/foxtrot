#pragma once
#include <memory>
#include <map>
#include <vector>
#include <climits>
#include <mutex>
#include <deque>
#include <variant>

#include <foxtrot/server/Device.h>
#include <foxtrot/ft_capability.grpc.pb.h>
#include <foxtrot/Logging.h>

using prop_or_meth = std::variant<rttr::property, rttr::method>;


namespace foxtrot
{
    
    class DeviceHarness
    {
    public:
        DeviceHarness();
        void AddDevice(std::unique_ptr<Device,void(*)(Device*)> dev);
        void AddDevice(std::unique_ptr<Device> dev);
        
        Device* const GetDevice(int id);
        
        void ClearDevices(unsigned contention_timeout_ms);
        
        
        std::vector<std::string> GetCapabilityNames(int devid);
        devcapability GetDeviceCapability(int devid, const std::string& capname);
        
        //TODO:must be a more elegant way to do this trick
        const std::map<int,const Device*> GetDevMap() const;
	std::unique_lock<std::timed_mutex> lock_device_contentious(int devid, unsigned contention_timeout_ms);
	
	
	
    private:          
      
        int _id = 0;
        foxtrot::Logging _lg;
//         std::map<int,std::unique_ptr<Device,void(*)(Device*)>> _devmap;
        std::vector<std::unique_ptr<Device,void(*)(Device*)>> _devvec;
//         std::deque<std::timed_mutex> _devmutexes;
        
        std::map<int, std::timed_mutex> _devmutexes;
        
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
