#include "WatchDogEvent.h"
#include <Logging.h>
#include <client/client.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <boost/variant.hpp>
#include "WatchDogHeater.h"
#include <iomanip>

using std::cout;
using std::endl;


class WDDummy : public foxtrot::WatchDogHeater
{
public:
  WDDummy() : WatchDogHeater("WDDummy", 10,1000)
  {}
  
  virtual bool action(foxtrot::Client& cl)
  {
    return false;
    
  }
  
  virtual bool check_trigger(foxtrot::Client& cl)
  {
    update_archon_state(cl);
    auto st = get_stage_temp(cl);
    auto tt = get_tank_temp(cl);
    auto ht = get_heater_target(cl);
    
    cout << " st: " << st << " tt: " << tt << " ht: " << ht << endl; 
    cout << "<moving avs> st: " <<  partial_average(stage_history,10) <<  " tt: " << partial_average(tank_history,10) << endl;
    cout << "<rate of change> st: " << rate_of_change(stage_history,10) << " tt: " << rate_of_change(tank_history,10) << endl;
    
    return false;
  }
  
};
  

int main(int argc, char** argv)
{
    foxtrot::Logging lg("watchdog");
    foxtrot::setDefaultSink();
    foxtrot::setLogFilterLevel(sl::trace);
    
    
    lg.Info("watchdog starting up");
    
    foxtrot::Client cl("localhost:50051");

    
    auto desc = cl.DescribeServer();
    
    
    auto wde = WDDummy();
    wde.setup_devices(desc);
    
    
    while(true) 
    {
	auto res = wde.check_trigger(cl);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }


}
