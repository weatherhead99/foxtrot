#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include "OPMD_setup.h"

//#include <plplot/plstream.h>
#include "wl_sweep_io.h"

// #include <root/TCanvas.h>
#include <utility>

#include <gsl/gsl_statistics.h>

using std::cout;
using std::endl;


std::pair<double,double> meanvar(std::vector<double> arr)
{
  
  double mean = gsl_stats_mean(arr.data(),1,arr.size());
  double variance = gsl_stats_variance(arr.data(),1,arr.size());
  
  return std::make_pair(mean,variance);
}



std::vector<double> take_readings(foxtrot::devices::newport2936R OPM, unsigned short num_readings, int delay_ms)
{
 std::vector<double> out;
 out.reserve(num_readings);

 for(short i =0; i < num_readings; i++)
 {
   out.push_back(OPM.getPower());
   std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
 }
  
  return out;
}

void psu_power_switch(foxtrot::devices::Q250 psu, bool onoff)
{
  double ampreset;
  
  if(onoff)
  {
    ampreset = psu.getAmpPreset();
    psu.start();
  cout << "powering up lamp..." << endl;
  }
  else
  {
    ampreset = 0;
    psu.stop();
    cout << "powering off lamp..." << endl;
  };
  
  auto amps = psu.getAmps();
  while(amps != ampreset )
  {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    amps = psu.getAmps();
    cout << "." ;
  }
  cout << endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  
  cout << "lamp power " << (onoff ? "up" : "down") << "complete" << endl;
  
};

int main(int argc, char** argv)
{
  unsigned num_readings = 100;
  OPMD_setup expt;
  
//   auto graph = new TCanvas();
  
  wl_sweep_table table("results.fits",num_readings);
  
  auto OPM = expt.getOPM();
  auto monoch = expt.getmonoch();
  auto psu = expt.getPSU();
  
  auto wait_ms = [] (int ms) {std::this_thread::sleep_for(std::chrono::milliseconds(ms));};
  
//   psu_power_switch(psu,true);
  
  monoch.setWave(300);
#ifndef NEW_RTTR_API
  OPM.setUnits(foxtrot::devices::powerunits::Amps);
  
#else
  OPM.setUnits(0);
#endif
  wait_ms(2000);
  
  psu_power_switch(psu,true);
  
  
  for(int wl = 250; wl <= 1000; wl += 10)
  {
   cout << "setting wl to: " << wl << " nm" << endl; 
   
   OPM.setLambda(wl);
   monoch.setWave(wl);
   
   //TODO: check if monoch is finished yet!
   wait_ms(1000);
   
   cout << "monoch wave is: " << monoch.getWave() << endl;
   
   
   auto readings = take_readings(OPM,num_readings,100);
   auto responsivity = OPM.getResponsivity();
   
//    cout << "readings: ";
//    for(auto c : readings)
//    {
//      cout << c << " ";
//    }
//    cout << endl;
//    
//    cout << "responsivity: " << responsivity;
   
   table.add_data_row(readings,responsivity,wl);
   
   
  }
  
  psu_power_switch(psu,false);
  
  
}
