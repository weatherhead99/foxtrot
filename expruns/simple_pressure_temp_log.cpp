
#include <boost/date_time.hpp>
#include <tuple>
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
#include "DeviceError.h"
#include "ProtocolError.h"
#include "client.h"
#include "Logging.h"

#include <boost/program_options.hpp>

using LocalProtocolError = typename foxtrot::ProtocolError;

namespace po = boost::program_options;


using std::string;
using std::cout;
using std::endl;
namespace pt = boost::posix_time;


double getPressure(foxtrot::Client& cl, int devid, int channel)
{
  auto response = cl.InvokeCapability(devid,"getPressure",{channel});

  
  return boost::get<double>(response);
};

std::tuple<double,double> getTemps(foxtrot::Client& cl, int archon_devid, int heater_devid)
{
  cl.InvokeCapability(archon_devid,"update_state");

  
  auto TA = boost::get<double>(cl.InvokeCapability(heater_devid,"getTempA"));
  auto TB = boost::get<double>(cl.InvokeCapability(heater_devid,"getTempB"));

  return std::make_tuple(TA,TB);
  
}

std::tuple<double,double> getHeater(foxtrot::Client& cl, int heater_devid)
{
    
    auto htr_output = boost::get<double>(cl.InvokeCapability(heater_devid,"getHeaterAOutput"));
    
    std::vector<foxtrot::ft_variant> args{0};
    auto htr_target = boost::get<double>(cl.InvokeCapability(heater_devid, "getHeaterTarget",args.begin(), args.end()));
    
    return std::make_tuple(htr_output,htr_target);
};



int main(int argc, char**argv)
{
  backward::SignalHandling sh;
  
  int debug_level;
  //program options setup
  po::options_description desc("Allowed Options");
  desc.add_options()("outfname",po::value<string>(),"set logging file name")
		  ("interval_s",po::value<int>()->default_value(120),"seconds between logging points")
		  ("fname",po::value<string>()->default_value("temp_pres_"),"filename base to store data")
		  ("debug,d",po::value<int>(&debug_level)->default_value(4),"set debug level");
		  
  po::variables_map vm;
  po::store(po::parse_command_line(argc,argv,desc),vm);

  po::notify(vm);
  
  auto interval_s = vm["interval_s"].as<int>();
  auto fname_base = vm["fname"].as<string>();
  
  foxtrot::Logging lg("simple_pressure_temp_log");
  foxtrot::setLogFilterLevel(static_cast<sl>(6 - debug_level));
  
  //setup log file
  //string folder = "/home/dweatherill/teststation_logs/";
  
  
  boost::gregorian::date current_date(boost::gregorian::day_clock::local_day());
  
  pt::time_facet fc ("%Y_%M_%d");
  
  std::ostringstream fname;
  fname <<  fname_base;
  fname.imbue(std::locale(fname.getloc(),&fc));
  fname << current_date;
  fname << ".txt";
  
  cout << fname.str() << endl;
  
  //check if file exists
  bool fexists;
  
  {
    std::fstream ifs(fname.str(),std::ios::in);
    fexists = ifs.good();
  };
  
  std::fstream fs(fname.str(),std::ios::out| std::ios::app);
  fs.exceptions(std::fstream::failbit | std::fstream::badbit);
  
  if(!fexists)
  {
  fs << "#unixtime,date/time,pressure_cryostat(hPa),pressure_pump(hPa),temperature_stage(C),temperature_tank(C),heater_output(V),heater_target(C)" << endl;
  };
  
  
  foxtrot::Client cl("localhost:50051");
  auto servdesc = cl.DescribeServer();
  lg.Info("server comment: " + servdesc.servcomment());
  
  
  auto pressure_gauge_devid = foxtrot::find_devid_on_server(servdesc,"TPG362");
  if(pressure_gauge_devid == -1)
  {
    lg.Fatal("no pressure gauge on server!");
    exit(1);
  }
  lg.Debug("pressure gauge devid: " + std::to_string(pressure_gauge_devid));
  
  
  auto archon_devid = foxtrot::find_devid_on_server(servdesc,"archon");
  if(archon_devid == -1)
  {
    lg.Fatal("no archon on server!");
    exit(1);
  }
  lg.Debug("archon devid: " + std::to_string(archon_devid));
  
  auto heater_devid = foxtrot::find_devid_on_server(servdesc,"ArchonHeaterX");
  if(heater_devid == -1)
  {
    lg.Fatal("no heater on server!");
    exit(1);
  }
  lg.Debug("heater devid: " + std::to_string(heater_devid));
  
  
  while(true)
  {
   
   auto now = pt::second_clock::local_time();
   auto pressure_pump = getPressure(cl,pressure_gauge_devid,2);
   auto pressure_cryostat = getPressure(cl,pressure_gauge_devid,1);
   
   double tank_temp = -273.15;
   double stage_temp = -273.15;
   double htr_output = 0.0;
   double htr_target = -273.15;
   try{
    
      auto output_target = getHeater(cl,heater_devid); 
      auto temps = getTemps(cl,archon_devid,heater_devid);
      tank_temp = std::get<0>(temps);
      stage_temp = std::get<1>(temps);
      htr_output = std::get<0>(output_target);
      htr_target = std::get<1>(output_target);
   }
#ifdef linux
   catch(typename foxtrot::ProtocolError)
#else
     catch(LocalProtocolError)
#endif
   {
     lg.Error( "archon seems to have failed.... " );
     lg.Error("logging only pressure...");
     
   }
   lg.Debug( "date time is: " + pt::to_simple_string(now) );
   lg.Debug( "pressure at cryostat is: " + std::to_string(pressure_cryostat) +  " hPa" );
   lg.Debug("pressure at pump is: " + std::to_string(pressure_pump) +  " hPa" );
   
   lg.Debug("LN2 tank temperature: " + std::to_string(tank_temp) + " degC ");
   lg.Debug("stage temperature: " + std::to_string(stage_temp) +  " degC ");
   
   lg.strm(sl::debug) << "heater output is: " << htr_output << " V";
   lg.strm(sl::debug) << "heater target is: " << htr_target << " degC";
   
   auto unix_epoch = (now - pt::from_time_t(0)).total_seconds();
   
   fs  << unix_epoch << "," << pt::to_iso_string(now) << "," <<pressure_cryostat <<","
   << pressure_pump << "," << stage_temp << "," << tank_temp << "," << htr_output << "," << htr_target << endl;// "," << res << "," << temperature << endl;
   fs.flush();
   
   lg.Debug("sleeping for " + std::to_string(interval_s) +  " seconds..." );
   
   std::this_thread::sleep_for(std::chrono::seconds(interval_s));
    
  }
  
  
  
}
