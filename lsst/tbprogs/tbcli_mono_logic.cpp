#include "tbcli_mono_logic.h"
#include <algorithm>


int find_monochromator(const foxtrot::servdescribe& cl)
{
  
  auto mono = std::find_if(cl.devs_attached().begin(), cl.devs_attached().end(),
			   [] (decltype(*cl.devs_attached().begin())& val)
			   {
			     if(val.second.devtype() == "cornerstone260")
			     {return true;
			     }
			     return false;
			   });
  
  if(mono == cl.devs_attached().end())
  {
    return -1;
  }
  
  return mono->first;

}


int get_filter(foxtrot::Client& cl, int devid)
{
  return std::get<int>(cl.InvokeCapability(devid,"getFilter"));
  
}


bool get_shutter(foxtrot::Client& cl, int devid)
{
  auto resp = cl.InvokeCapability(devid,"getShutterStatus");
  return std::get<bool>(resp);

}

double get_wavelength(foxtrot::Client& cl, int devid)
{
  auto resp = cl.InvokeCapability(devid,"getWave");
  return std::get<double>(resp);

}

void set_wavelength_dumb(foxtrot::Client& cl, int devid, double wl)
{
  std::vector<foxtrot::ft_std_variant> args{wl};
  cl.InvokeCapability(devid,"setWave",args.begin(), args.end());
}

void set_filter_dumb(foxtrot::Client& cl, int devid, int filter)
{
  std::vector<foxtrot::ft_std_variant> args{filter};
  cl.InvokeCapability(devid,"setFilter",args.begin(), args.end());
}


void set_shutter_dumb(foxtrot::Client& cl, int devid, bool onoff)
{
  std::vector<foxtrot::ft_std_variant> args{onoff};
  cl.InvokeCapability(devid,"setShutterStatus",args.begin(), args.end());
  
}

int get_grating(foxtrot::Client& cl, int devid)
{
  return std::get<int>(cl.InvokeCapability(devid,"getGrating"));

}

void set_grating_dumb(foxtrot::Client& cl, int devid, int grating)
{
  std::vector<foxtrot::ft_std_variant> args{grating};
  cl.InvokeCapability(devid,"setGrating", args.begin(), args.end());

}

void set_wavelength_smart(foxtrot::Client& cl, int devid, double wl)
{
  bool wait=false;
  
  //set grating
  if(wl < 675.0)
    if(get_grating(cl,devid) != 1) {wait=true;set_grating_dumb(cl,devid,1);}
  else
    if(get_grating(cl,devid) !=2) {wait =true;set_grating_dumb(cl,devid,2);}
    
  //set filter
  if(wl <= 355.0)
    set_filter_dumb(cl,devid,1);
  else if(wl <= 625.0)
    set_filter_dumb(cl,devid,2);
  else if(wl <= 1100.0)
    set_filter_dumb(cl,devid,3);
  else if(wl <= 1500.0)
    set_filter_dumb(cl,devid,4);
  else
    set_filter_dumb(cl,devid,5);
  
  set_wavelength_dumb(cl,devid,wl);

}

