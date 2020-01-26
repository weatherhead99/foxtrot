#pragma once
#include <tinyfsm.hpp>

namespace foxtrot
{
namespace fsmd
{
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
        bool experiment_lowvibration;
        
    };

    struct TimeTickEvent : public tinyfsm::Event 
    {
        
    };
    
}
}
