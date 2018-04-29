#pragma once
#include <string>
#include <client.h>
#include <Logging.h>


namespace foxtrot {
    class WatchDogEvent
    {
    public:
        WatchDogEvent(const std::string& eventname, unsigned tick_check);
        
        virtual void setup_devices(const foxtrot::servdescribe& desc) = 0;
        virtual bool check_trigger(foxtrot::Client& cl) = 0;
        virtual bool action(foxtrot::Client& cl) = 0;
        
    private:
        std::string eventname_;
        foxtrot::Logging lg_;
            
    };   
    
}
