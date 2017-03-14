#include "tbcli_pressure_logic.h"
#include <algorithm>

int find_pressure_gauge(const foxtrot::servdescribe& cl)
{
 
    auto pressure = std::find_if(cl.devs_attached().begin(), cl.devs_attached().end(),
                               [] (decltype(*cl.devs_attached().begin())& val) 
                               {
                                   if(val.second.devtype() == "TPG362")
                                   {
                                       return true;
                                   }
                                   return false;
                            }
                       );
    
    if(pressure == cl.devs_attached().end())
    {
        return -1;
    }
    
    return pressure->first;
    
    
}

double get_cryostat_pressure(foxtrot::Client& cl, int devid)
{
    std::vector<foxtrot::ft_variant> args{1};
    auto pres = boost::get<double>(cl.InvokeCapability(devid,"getPressure",args.begin(),args.end()));
    
    return pres;
}

double get_pump_pressure(foxtrot::Client& cl, int devid)
{
    std::vector<foxtrot::ft_variant> args{2};
    auto pres = boost::get<double>(cl.InvokeCapability(devid,"getPressure",args.begin(), args.end()));
    
    return pres;
    
}
