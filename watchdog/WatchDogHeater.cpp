#include "WatchDogHeater.h"
#include <algorithm>


foxtrot::WatchDogHeater::WatchDogHeater(const std::string& name, int tick_check, int history_limit)
: WatchDogEvent(name, tick_check), history_limit_(history_limit)
{
}

void foxtrot::WatchDogHeater::setup_devices(const foxtrot::servdescribe& desc)
{
    devid_heater_ = foxtrot::find_devid_on_server(desc,"ArchonHeaterX");
    if(devid_heater_ < 0)
    {
        throw std::runtime_error("couldn't find heater on server!");
    }
    
}

double foxtrot::WatchDogHeater::get_heater_target(foxtrot::Client& cl)
{
    std::vector<foxtrot::ft_variant> args{0};
    auto resp = cl.InvokeCapability(devid_heater_,"getHeaterTarget",args.begin(), args.end());
    return boost::get<double>(resp);
}

double foxtrot::WatchDogHeater::get_stage_temp(foxtrot::Client& cl)
{
    auto resp = cl.InvokeCapability(devid_heater_,"getTempA");
    store_value(stage_history, boost::get<double>(resp));
    return boost::get<double>(resp);
}

double foxtrot::WatchDogHeater::get_tank_temp(foxtrot::Client& cl)
{
    auto resp = cl.InvokeCapability(devid_heater_,"getTempB");
    store_value(tank_history, boost::get<double>(resp));
    return boost::get<double>(resp);
}

void foxtrot::WatchDogHeater::set_heater_target(foxtrot::Client& cl, double target)
{
    std::vector<foxtrot::ft_variant> args{0, target};
    cl.InvokeCapability(devid_heater_, "setHeaterTarget",args.begin(), args.end());
}


void foxtrot::WatchDogHeater::store_value(std::deque<double>& history, double val)
{
    while(history.size() >= history_limit_)
    {
        history.pop_front();
    }
    
    history.push_back(val);
}


double foxtrot::WatchDogHeater::partial_average(const std::deque<double>& history, int average_size)
{
    auto sum = std::accumulate(history.end() - average_size, history.end(), 0);
    return sum / average_size;
    
}

double foxtrot::WatchDogHeater::rate_of_change(const std::deque<double>& history, int size)
{
    auto diff = std::accumulate(history.rend(), history.rend() + size, 0);
    return diff / size;
    
}

bool foxtrot::WatchDogHeater::rising(const std::deque<double>& history, int size)
{
  auto rate = rate_of_change(history,size);
  return rate > 0;
    
};


foxtrot::KeepStageWarm::KeepStageWarm(int tick_check, int history_limit, int average_size)
: average_size_(average_size)
{
}

bool foxtrot::KeepStageWarm::check_trigger(foxtrot::Client& cl)
{
}

bool foxtrot::KeepStageWarm::action(foxtrot::Client& cl)
{
}

