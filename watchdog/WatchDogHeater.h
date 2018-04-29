#pragma once
#include "WatchDogEvent.h"

namespace foxtrot {
    
    class WatchDogHeater : public WatchDogEvent
    {
    public:
        WatchDogHeater(const std::string& name, int tick_check);
        void setup_devices(const int & desc) override;
        
    private:
        int devid_heater_;
        
    };
    
    
}
