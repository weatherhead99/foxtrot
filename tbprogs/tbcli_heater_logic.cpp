#include "tbcli_heater_logic.h"
#include <algorithm>


bool find_archon_heater(const foxtrot::servdescribe& cl)
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
    
    return false;
    
}

void enable_disable_heater_output(foxtrot::Client& cl)
{
    std::vector<foxtrot::ft_variant> args{2};
    
    
}


