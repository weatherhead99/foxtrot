#include "tbcli_fill_logic.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>

int find_webswitch(const foxtrot::servdescribe& sd)
{
    auto webswitch = std::find_if(sd.devs_attached().begin(), sd.devs_attached().end(),
                                  [] (decltype(*sd.devs_attached().begin())& val)
                                  {
                                     if(val.second.devtype() == "webswitch_plus")
                                         return true;
                                     return false;                                   
                                  });
    
    if(webswitch == sd.devs_attached().end())
      return -1;
    
    return webswitch->first;
    
};


void do_fill(foxtrot::Client& cl, int devid, double filltime_hours, int relay)
{
    std::cout << "energising relay..." << std::endl;
    std::vector<foxtrot::ft_variant> args{relay, true};
    
    cl.InvokeCapability(devid,"SetRelay",args.begin(),args.end());
    std::cout << "sleeping for fill time: " << filltime_hours << " hours" << std::endl;
    
    //NOTE: rounds down, don't care
    auto seconds = std::chrono::seconds((int) (filltime_hours * 3600) );
    std::this_thread::sleep_for(seconds);
    
    std::cout << "fill should have finished, turning off relay..." << std::endl;
    
    args[1] = false;
    
    cl.InvokeCapability(devid, "SetRelay", args.begin(), args.end());
    
    
};
