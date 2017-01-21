
#include <boost/date_time.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>

#include "devices/TPG362/TPG362.h"
#include "SerialPort.h"
#include "characterdevice.h"
#include "PRT.h"

#include "devices/DM3068/DM3068.h"

using std::string;
using std::cout;
using std::endl;
namespace pt = boost::posix_time;

const int interval_s = 120;


foxtrot::parameterset dm_params {
  {"devnode" , "/dev/usbtmc0"}
};


foxtrot::parameterset tpg_params {
  {"port" , "/dev/ttyUSB0"},
  {"baudrate" , 9600u},
  };
 


int main(int argc, char**argv)
{
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&tpg_params);
  foxtrot::devices::TPG362 vacuumgauge(sport);
  
//   auto usbtmc = std::make_shared<foxtrot::protocols::characterdevice>(&dm_params);
//   foxtrot::devices::DM3068 multimeter(usbtmc);
  
  
  //setup log file
  string folder = "/home/dweatherill/teststation_logs/";
  string fname_base = "temp_pres_";
  
  boost::gregorian::date current_date(boost::gregorian::day_clock::local_day());
  
  pt::time_facet fc ("%Y_%M_%d");
  
  std::ostringstream fname;
  fname << folder + fname_base;
  fname.imbue(std::locale(fname.getloc(),&fc));
  fname << current_date;
  fname << ".txt";
  
  cout << fname.str() << endl;
  
  std::fstream fs(fname.str(),std::ios::out);
  fs.exceptions(std::fstream::failbit | std::fstream::badbit);
  
  fs << "date/time,pressure(hPa),resistance(ohm),temperature(C)" << endl;
  
  while(true)
  {
   
    
   auto now = pt::second_clock::local_time();
   auto pressure = vacuumgauge.getPressure(2);
//    auto res = multimeter.get4WireResistance();
//    auto temperature = foxtrot::util::ITL_90_res_to_tmp(res,foxtrot::util::PRTsensors::PT100);
   cout << "date time is: " << pt::to_simple_string(now) << endl;
   cout << "pressure: " << pressure << "hPa" <<  endl;
   
//    cout << "resistance: " << res << "ohms" << endl;
//    cout << "temperature: " <<  temperature << endl;
   
   cout << "sleeping for " << interval_s << " seconds..." <<endl;
   
   std::this_thread::sleep_for(std::chrono::seconds(interval_s));
    
   fs << pt::to_iso_string(now) << "," << pressure << endl;// "," << res << "," << temperature << endl;
   fs.flush();
   
   
    
  }
  
  
  
}