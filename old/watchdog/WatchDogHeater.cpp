#include "WatchDogHeater.h"
#include <algorithm>
#include <numeric>


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
    
    devid_archon_ = foxtrot::find_devid_on_server(desc,"archon");
    if(devid_archon_ < 0)
    {
      throw std::runtime_error("couldn't find archon on server!");
    }
    
}

double foxtrot::WatchDogHeater::get_heater_target(foxtrot::Client& cl)
{
    std::vector<foxtrot::ft_variant> args{0};
    auto resp = cl.InvokeCapability(devid_heater_,"getHeaterTarget",args.begin(), args.end());
    return boost::get<double>(resp);
}

double foxtrot::WatchDogHeater::get_stage_temp(foxtrot::Client& cl, bool store)
{
    auto resp = cl.InvokeCapability(devid_heater_,"getTempA");
    if(store)
      store_value(stage_history, boost::get<double>(resp));
    return boost::get<double>(resp);
}

double foxtrot::WatchDogHeater::get_tank_temp(foxtrot::Client& cl, bool store)
{
    auto resp = cl.InvokeCapability(devid_heater_,"getTempB");
    if(store)
      store_value(tank_history, boost::get<double>(resp));
    return boost::get<double>(resp);
}


void foxtrot::WatchDogHeater::update_archon_state(foxtrot::Client& cl)
{
  cl.InvokeCapability(devid_archon_, "update_state");

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
    if( history.size() < average_size )
	average_size = history.size();
    auto sum = std::accumulate(history.end() - average_size, history.end(), 0.0);
    lg().strm(sl::trace) << "sum: " << sum;
    lg().strm(sl::trace) << "av: " << sum / average_size;
    return sum / average_size;
    
}

double foxtrot::WatchDogHeater::rate_of_change(const std::deque<double>& history, int size)
{
    if( history.size() < size )
      size = history.size();
    
    if( size ==1 )
      return history.back();
    
    lg().strm(sl::trace) << "size: " << size;
  
    double diff = *history.rbegin() - *(history.rbegin() + (size-1)) ;
    
    lg().strm(sl::trace) << "diff: " << diff ;
    lg().strm(sl::trace) << "RoC: " << diff / size;
    return diff / size;
    
}

bool foxtrot::WatchDogHeater::rising(const std::deque<double>& history, int size)
{
  auto rate = rate_of_change(history,size);
  
  return rate > 0;
    
};


foxtrot::KeepStageWarm::KeepStageWarm(int tick_check, int history_limit, int average_size)
: WatchDogHeater("KeepStageWarm",tick_check,history_limit), average_size_(average_size)
{
}

bool foxtrot::KeepStageWarm::check_trigger(foxtrot::Client& cl)
{
  update_archon_state(cl);
  auto st = get_stage_temp(cl);
  auto tt = get_tank_temp(cl);
  auto ht = get_heater_target(cl);
  if( rising(tank_history,average_size_) && ( st - tt) < 10.0 )
  {
    lg().strm(sl::debug) << "tank temp rising and stage differential < 10!";
    if( ht < (tt + 20) )
    {
      lg().strm(sl::debug) << "heater target not high enough, triggering!";
    }
    return true;
  }
  
  return false;
    
}

bool foxtrot::KeepStageWarm::action(foxtrot::Client& cl)
{
  auto tt = get_tank_temp(cl,false);
  lg().strm(sl::warning) << "triggered KeepStageWarm, adjusting heater target!";
  set_heater_target(cl, tt + 30);

  //TODO:FIX THIS!
  return false;
  
}

