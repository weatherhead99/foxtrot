#pragma once
#include "Device.h"

#include <random>

#include <rttr/type>

namespace foxtrot
{
    namespace devices
    {
    class dummyDevice : public foxtrot::Device 
    {
        RTTR_ENABLE(foxtrot::Device)
    public:
        dummyDevice();
        
        double getRandomDouble() ;
        int getCounter();
        
        void resetCounter();
        
            
        const std::string getDeviceTypeName() const override;
        
    protected:
        
        
    private:
        std::default_random_engine _generator;
        std::normal_distribution<double> _distribution;
        
        int _counter = 0;
        
        
    };
    
    
    }
    
}


