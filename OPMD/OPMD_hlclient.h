#pragma once
#include <string>
#include <Logging.h>
#include <array>

namespace foxtrot
{
  class Client;
}


class OPMD_hlclient
{
public:
  OPMD_hlclient(const std::string& connstr);
  virtual ~OPMD_hlclient();
  
  
  
  double get_cryostat_pressure();
  double get_pump_pressure();
  
  double get_stage_temp();
  double get_tank_temp();
  
  void set_heater_onoff(bool onoff);
  bool get_heater_onoff();
  
  double get_heater_output();
  std::array<int,3> get_heater_PID();
  
  double get_heater_target();
  void set_heater_target(double tgt);
  
  void apply_heater_settings();
  
  
private:
  int _heater_devid = -1;
  int _pressure_devid = -1;
  
  foxtrot::Logging _lg;
  
  foxtrot::Client* _client;
  
};
