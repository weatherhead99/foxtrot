#pragma once
#include <boost/date_time.hpp>

namespace foxtrot {

struct env_data {
    boost::posix_time::ptime timestamp;
    double tank_temp;
    double stage_temp;
    double cryostat_pressure;
    double pump_pressure;
    double heater_output;
    double heater_target;
};
    
    
enum class event_types {
    fill_begin,
    fill_complete,
    tank_empty,
    dewar_empty,
    dewar_filled
};


struct event_data {
    boost::posix_time::ptime timestamp;
    event_types evtp;
};


}
