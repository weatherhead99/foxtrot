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
    cl.InvokeCapability(devid,"apply");
    
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

void update_archon_state(const foxtrot::servdescribe& cl, foxtrot::Client& cli)
{
      auto archon = std::find_if(cl.devs_attached().begin(), cl.devs_attached().end(),
                               [] (decltype(*cl.devs_attached().begin())& val) 
                               {
                                   if(val.second.devtype() == "archon")
                                   {
                                       return true;
                                   }
                                   return false;
                            }
                       );
  
    if(archon == cl.devs_attached().end())
      {
	  throw std::logic_error("no archon connected to server");
      }
      
      
   cli.InvokeCapability(archon->first, "update_state");
}

std::vector< int > get_heater_coeffs(foxtrot::Client& cl, int devid)
{
  auto P = boost::get<int>(cl.InvokeCapability(devid,"getHeaterAP"));
  auto I = boost::get<int>(cl.InvokeCapability(devid,"getHeaterAI"));
  auto D = boost::get<int>(cl.InvokeCapability(devid,"getHeaterAD"));
  
  return std::vector<int>{P,I,D};

}

double get_heater_output(foxtrot::Client& cl, int devid)
{
  return boost::get<double>(cl.InvokeCapability(devid, "getHeaterAOutput"));
  
}




std::pair< double, double > get_temperatures(foxtrot::Client& cl, int devid)
{
  std::pair<double,double> out;
  
  out.first = boost::get<double>(cl.InvokeCapability(devid,"getTempA"));
  out.second = boost::get<double>(cl.InvokeCapability(devid,"getTempB"));
  
  return out;

}



void apply_settings(foxtrot::Client& cl, int devid)
{
  std::vector<foxtrot::ft_variant> args{};
  cl.InvokeCapability(devid,"apply", args.begin(), args.end());
  
}

