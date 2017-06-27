#pragma once
#include "Device.h"
#include <memory>
#include <map>
#include <vector>
#include <climits>
#include "foxtrot.grpc.pb.h"
#include <mutex>
#include <deque>
#include "Logging.h"

namespace foxtrot
{
    
    class DeviceHarness
    {
    public:
        DeviceHarness();
        void AddDevice(std::unique_ptr<Device,void(*)(Device*)> dev);
        void AddDevice(std::unique_ptr<Device> dev);
        
        Device* const GetDevice(int id);
        std::timed_mutex& GetMutex(int id);
        
        std::vector<std::string> GetCapabilityNames(int devid);
        devcapability GetDeviceCapability(int devid, const std::string& capname);
        
        
        //TODO:must be a more elegant way to do this trick
        const std::map<int,const Device*> GetDevMap() const;
        
	rttr::variant call_capability(int devid, rttr::property& prop, unsigned contention_timeout_ms);
	rttr::variant call_capability(int devid, rttr::property& prop, rttr::variant arg, unsigned contention_timeout_ms);
        rttr::variant call_capability(int devid, rttr::method& meth, std::vector<rttr::variant>& args, unsigned contention_timeout_ms);
	std::unique_lock<std::timed_mutex> lock_device_contentious(int devid, unsigned contention_timeout_ms);
	
    private:
      
      
        int _id = 0;
        foxtrot::Logging _lg;
//         std::map<int,std::unique_ptr<Device,void(*)(Device*)>> _devmap;
        std::vector<std::unique_ptr<Device,void(*)(Device*)>> _devvec;
        std::deque<std::timed_mutex> _devmutexes;
        
    };
    

}
