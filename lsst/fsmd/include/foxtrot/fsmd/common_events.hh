#pragma once
#include <tinyfsm.hpp>
#include <chrono>

namespace foxtrot
{
namespace fsmd
{
    using timept = std::chrono::time_point<std::chrono::system_clock>;
    using duration_s = std::chrono::seconds;
    
    struct DataUpdateEvent : public tinyfsm::Event
    {
        //environmental data
        double pump_pressure;
        double cryo_pressure;
        double tank_temp;
        double stage_temp;
        double pump_rot_percent;
        int pump_rot_speed;
        bool pump_venting;
        bool valve_open;
        bool pump_enabled;
        
        //server flags
        bool autofill_enabled;
        bool experiment_running;
        bool lowvibration_requested;
        bool tank_refill_flag_set;
        
        
    };

    struct TimeTickEvent : public tinyfsm::Event 
    {
        
    };
    
    
    
    
}
}
