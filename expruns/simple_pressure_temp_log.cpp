
#include <boost/date_time.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <backward.hpp>
#include "devices/TPG362/TPG362.h"
#include "SerialPort.h"
#include "characterdevice.h"
#include "simpleTCP.h"
#include "PRT.h"

#include "devices/DM3068/DM3068.h"
#include <archon/archon.h>
#include <archon/archon_module_heaterx.h>
#include <DeviceError.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

using std::string;
using std::cout;
using std::endl;
namespace pt = boost::posix_time;



foxtrot::parameterset dm_params {
  {"devnode" , "/dev/usbtmc0"}
};


foxtrot::parameterset tpg_params {
  {"port" , "/dev/ttyUSB0"},
  {"baudrate" , 9600u},
  };
 

foxtrot::parameterset archon_params {
  {"addr" , "10.0.0.2"},
  {"port" , 4242u}
};

std::vector<string> config_lines
{ {"MOD11/SENSORACURRENT=10000"},
{"MOD11/SENSORALABEL=TANK"},
{"MOD11/SENSORALOWERLIMIT=-150.0"},
{"MOD11/SENSORATYPE=2"},
{"MOD11/SENSORAUPPERLIMIT=50.0"},
{"MOD11/SENSORBCURRENT=10000"},
{"MOD11/SENSORBLABEL=STAGE"},
{"MOD11/SENSORBUPPERLIMIT=50.0"},
{"MOD11/SENSORBLOWERLIMIT=-150.0"},
{"MOD11/SENSORBTYPE=2"},
{"LINES=0"},
{"STATES=0"},
{"PARAMETERS=0"},
{"CONSTANTS=0"}
};

int main(int argc, char**argv)
{
  backward::SignalHandling sh;
  
  //program options setup
  po::options_description desc("Allowed Options");
  desc.add_options()("outfname",po::value<string>(),"set logging file name")
		  ("interval_s",po::value<int>()->default_value(120),"seconds between logging points");
		  
  po::variables_map vm;
  po::store(po::parse_command_line(argc,argv,desc),vm);

  po::notify(vm);
  
  auto interval_s = vm["interval_s"].as<int>();
  
  
  
  
  
  
  auto sport = std::make_shared<foxtrot::protocols::SerialPort>(&tpg_params);
  foxtrot::devices::TPG362 vacuumgauge(sport);
  
//   auto usbtmc = std::make_shared<foxtrot::protocols::characterdevice>(&dm_params);
//   foxtrot::devices::DM3068 multimeter(usbtmc);
  cout << "socket init..>" << endl;
  auto sock = std::make_shared<foxtrot::protocols::simpleTCP>(&archon_params);
  
  cout << "archon init.." << endl;
  foxtrot::devices::archon archon(sock);
  
  cout << "clearing config.." << endl;
  archon.clear_config();
  
  cout << "applying simple archon config script..." << endl;
  for(const auto& s : config_lines)
  {
    archon.writeConfigLine(s);
  }
  
  try{
    archon.applyall();
  }
  catch(foxtrot::DeviceError& err)
  {
    cout << "archon error logs follow :" << endl;
    auto all_logs = archon.fetch_all_logs();
    for(auto& log: all_logs)
    {
      cout << log << endl;
    };
    
    throw err;
    
  }
    
  auto modules = archon.getAllModules();
  auto heater = static_cast<const foxtrot::devices::ArchonHeaterX*>(&modules.at(10));
  
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
  
  fs << "unixtime,date/time,pressure(hPa),temperature_stage(C),temperature_tank(C)" << endl;
  
  while(true)
  {
   
    archon.update_state();
    
    
   auto now = pt::second_clock::local_time();
   auto pressure = vacuumgauge.getPressure(2);
   auto tank_temp = heater->getTempA();
   auto stage_temp = heater->getTempB();
   
//    auto res = multimeter.get4WireResistance();
//    auto temperature = foxtrot::util::ITL_90_res_to_tmp(res,foxtrot::util::PRTsensors::PT100);
   cout << "date time is: " << pt::to_simple_string(now) << endl;
   cout << "pressure: " << pressure << "hPa" <<  endl;
   
   cout << "LN2 tank temperature: " << tank_temp << " degC " << endl;
   cout << "stage temperature: " << stage_temp << " degC " << endl;
   
   
//    cout << "resistance: " << res << "ohms" << endl;
//    cout << "temperature: " <<  temperature << endl;
   auto unix_epoch = (now - pt::from_time_t(0)).total_seconds();
   
   fs  << unix_epoch << "," << pt::to_iso_string(now) << "," << pressure << "," << stage_temp << "," << tank_temp << endl;// "," << res << "," << temperature << endl;
   fs.flush();
   
   
   cout << "sleeping for " << interval_s << " seconds..." <<endl;
   
   std::this_thread::sleep_for(std::chrono::seconds(interval_s));
    
   
    
  }
  
  
  
}