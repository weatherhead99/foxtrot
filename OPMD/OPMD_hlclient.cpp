#include "OPMD_hlclient.h"
#include "client.h"


OPMD_hlclient::OPMD_hlclient(const std::string& connstr)
: _lg("OPMD_hlclient")
{
  
  _client = new foxtrot::Client(connstr);
  
  auto servdesc = _client->DescribeServer();
  _heater_devid = foxtrot::find_devid_on_server(servdesc, "ArchonHeaterX");
  _pressure_devid = foxtrot::find_devid_on_server(servdesc, "TPG362");
  
  if(_heater_devid < 0 || _pressure_devid < 0)
  {
    _lg.Error("heater devid: " + std::to_string(_heater_devid));
    _lg.Error("pressure devid: " + std::to_string(_pressure_devid));
    throw std::runtime_error("required devices not found on server");
    
  }
  

}


OPMD_hlclient::~OPMD_hlclient()
{
  if(_client)
  {
    delete _client;
  }

}

double OPMD_hlclient::get_cryostat_pressure()
{
   auto pres = boost::get<double>(_client->InvokeCapability(_pressure_devid,"getPressure",{1}));
    return pres;

}


double OPMD_hlclient::get_pump_pressure()
{
    return boost::get<double>(_client->InvokeCapability(_pressure_devid,"getPressure",{2}));
}

double OPMD_hlclient::get_heater_output()
{
  return boost::get<double>(_client->InvokeCapability(_heater_devid,"getHeaterAOutput"));
}

std::array< int, int(3) > OPMD_hlclient::get_heater_PID()
{
  auto P = boost::get<int>(_client->InvokeCapability(_heater_devid,"getHeaterAP"));
  auto I = boost::get<int>(_client->InvokeCapability(_heater_devid,"getHeaterAI"));
  auto D = boost::get<int>(_client->InvokeCapability(_heater_devid,"getHeaterAD"));
  
  return {P,I,D};
}

double OPMD_hlclient::get_heater_target()
{
  return boost::get<double>(_client->InvokeCapability(_heater_devid,"getHeaterTarget",{0}));
}

double OPMD_hlclient::get_stage_temp()
{
  return boost::get<double>(_client->InvokeCapability(_heater_devid,"getTempB")); 
}

double OPMD_hlclient::get_tank_temp()
{
  return boost::get<double>(_client->InvokeCapability(_heater_devid,"getTempA"));
}

void OPMD_hlclient::set_heater_onoff(bool onoff)
{
  _client->InvokeCapability(_heater_devid,"setHeaterEnable",{0, onoff});
  _client->InvokeCapability(_heater_devid, "apply");
  
}

bool OPMD_hlclient::get_heater_onoff()
{
  return boost::get<bool>(_client->InvokeCapability(_heater_devid,"getHeaterEnable",{0}));

}


