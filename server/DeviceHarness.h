#pragma once
#include "Device.h"
#include <memory>
#include <map>
#include <vector>
#include <climits>
#include "foxtrot.grpc.pb.h"
#include <rttr/type>
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
        std::mutex& GetMutex(int id);
        
        std::vector<std::string> GetCapabilityNames(int devid);
        devcapability GetDeviceCapability(int devid, const std::string& capname);
        
        
        //TODO:must be a more elegant way to do this trick
        const std::map<int,const Device*> GetDevMap() const;
        
        
    private:
        int _id = 0;
        foxtrot::Logging _lg;
//         std::map<int,std::unique_ptr<Device,void(*)(Device*)>> _devmap;
        std::vector<std::unique_ptr<Device,void(*)(Device*)>> _devvec;
        std::deque<std::mutex> _devmutexes;
        
    };
    

}
