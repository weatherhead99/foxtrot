#include "tbcli_heater_logic.h"
#include <algorithm>


int find_archon_heater(const foxtrot::servdescribe& cl)
{
    
    auto heater = std::find_if(cl.devs_attached().begin(), cl.devs_attached().end(),
                               [] (decltype(*cl.devs_attached().begin())& val) 
                               {
                                   if(val.second.devtype() == "ArchonHeaterX")
                                   {
                                       return true;
                                   }
                                   return false;
                            }
                       );
    
    if(heater == cl.devs_attached().end())
    {
        return -1;
    }
    
    return heater->first;
    
    
}

void enable_disable_heater_output(foxtrot::Client& cl, int devid, bool onoff)
{
    std::vector<foxtrot::ft_variant> args{0, onoff  };
    cl.InvokeCapability(devid,"setHeaterEnable",args.begin(), args.end());
    
}


bool is_heater_enabled(foxtrot::Client& cl, int devid)
{
    std::vector<foxtrot::ft_variant> args{0};
    auto resp = cl.InvokeCapability(devid,"getHeaterEnable",args.begin(), args.end());
    return boost::get<bool>(resp);
}


double get_heater_target(foxtrot::Client& cl, int devid)
{
    std::vector<foxtrot::ft_variant> args{0};
    auto resp = cl.InvokeCapability(devid,"getHeaterTarget",args.begin(), args.end());
    return boost::get<double>(resp);
    
}

void set_heater_target(foxtrot::Client& cl, int devid, double target)
{
    std::vector<foxtrot::ft_variant> args{0, target};
    cl.InvokeCapability(devid,"setHeaterTarget", args.begin(), args.end());
    
}



