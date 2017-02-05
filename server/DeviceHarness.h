#pragma once
#include "Device.h"
#include <memory>
#include <map>
#include <random>
#include <climits>

namespace foxtrot
{
    class DeviceHarness
    {
    public:
        DeviceHarness();
        void AddDevice(std::unique_ptr<Device> dev);
        
        Device* const GetDevice(int id);
        
        //TODO:must be a more elegant way to do this trick
        const std::map<int,const Device*> GetDevMap() const;
        
        
    private:
        std::map<int,std::unique_ptr<Device>> _devmap;
        std::random_device _generator;
        std::uniform_int_distribution<int> _distribution;
        
        
        
        
    };
    

}
