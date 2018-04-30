#pragma once
#include "WatchDogEvent.h"
#include <client.h>
#include <deque>

namespace foxtrot {
    
    class WatchDogHeater : public WatchDogEvent
    {
    public:
        WatchDogHeater(const std::string& name, int tick_check, int history_limit);
        void setup_devices(const foxtrot::servdescribe& desc) override;
    
    protected:
        double get_stage_temp(foxtrot::Client& cl);
        double get_tank_temp(foxtrot::Client& cl);
        double get_heater_target(foxtrot::Client& cl);
        
        double partial_average(const std::deque<double>& history, int average_size);
        double rate_of_change(const std::deque<double>& history, int size);
        
        bool rising(const std::deque<double>& history, int size);
        
    private:
        void store_value(std::deque<double>& history, double val);
        int history_limit_;
        int devid_heater_;
        std::deque<double> stage_history;
        std::deque<double> tank_history;
        
    };
    
    
    
    
    
}
