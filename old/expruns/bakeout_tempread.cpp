#include <iostream>
#include <backward.hpp>
#include "devices/DM3068/DM3068.h"
#include "devices/TPG362/TPG362.h"
#include "util/PRT/PRT.h"
#include "protocols/characterdevice.h"
#include "protocols/SerialPort.h"
#include <memory>
#include <boost/date_time.hpp>
#include <fstream>
#include <thread>
#include <chrono>

foxtrot::parameterset dm_params {
  {"devnode" , "/dev/usbtmc1"}
};

foxtrot::parameterset tpg_params {
  {"port" , "/dev/ttyUSB0"},
  {"baudrate" , 9600u},
  };


namespace pt = boost::posix_time;


int main(int argc, char** argv)
{
  backward::SignalHandling sh;
  
  auto proto = std::make_shared<foxtrot::protocols::characterdevice>(&dm_params);
  
  auto proto2 = std::make_shared<foxtrot::protocols::SerialPort> (&tpg_params);
  
  foxtrot::devices::DM3068 multimeter(proto);
  foxtrot::devices::TPG362 presgauge(proto2);
  
  std::fstream ofs("bakeout_log.txt", std::ios::out | std::ios::app);
  ofs.exceptions(std::fstream::failbit | std::fstream::badbit);
  
  ofs << "unixtime,date/time,temperature(C),pressure(hPa)" << std::endl;
  
  while(true)
  {
    auto now = pt::second_clock::local_time();
    auto resistance = multimeter.get4WireResistance();
    auto temperature = foxtrot::util::ITL_90_res_to_tmp(resistance,foxtrot::util::PRTsensors::PT100);
    auto pressure = presgauge.getPressure(1);
    auto unix_epoch = (now - pt::from_time_t(0)).total_seconds();
    
    ofs << unix_epoch << "," << pt::to_iso_string(now) << "," << temperature << "," << pressure << std::endl;
    ofs.flush();
    
    std::cout << pt::to_simple_string(now) << ": " << temperature << " degC" << ", " << pressure << " hPa"<< std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
  }
  

}