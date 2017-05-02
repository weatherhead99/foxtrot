#include "Q250.h"
#include <chrono>
#include <thread>

using namespace foxtrot;



const foxtrot::parameterset Q250_class_parameters
{
  {"LBA", 100000u}
};

foxtrot::devices::Q250::Q250(std::shared_ptr<protocols::scsiserial> proto)
: CmdDevice(proto), _scsiproto(proto)
{
  _scsiproto->Init(&Q250_class_parameters);
}

const std::string devices::Q250::getDeviceTypeName() const
{
    return "Q250";
}


std::string foxtrot::devices::Q250::cmd(const std::string& request)
{
  
  _scsiproto->write(request);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  auto repl  = _scsiproto->read(512);
  return repl;
}

void devices::Q250::start()
{
  _scsiproto->write("START");

}

void devices::Q250::stop()
{
  _scsiproto->write("STOP");

}


double devices::Q250::getAmps()
{
  auto repl = cmd("AMPS?");
  return std::stod(repl);
}

double devices::Q250::getAmpLimit()
{
  auto repl = cmd("A-LIM?");
  return std::stod(repl);
}

double devices::Q250::getAmpPreset()
{
  auto repl = cmd("A-PRESET?");
  return std::stod(repl);
}


double devices::Q250::getVolts()
{
  auto repl = cmd("VOLTS?");
  return std::stod(repl);
}

int devices::Q250::getWatts()
{
  auto repl = cmd("WATTS?");
  return std::stoi(repl);
}



RTTR_REGISTRATION
{
 using namespace rttr;
using devices::Q250;
  registration::class_<Q250>("foxtrot::devices::Q250")
  .property_readonly("getWatts", &Q250::getWatts)
  .property_readonly("getVolts", &Q250::getVolts)
  .property_readonly("getAmpPreset",&Q250::getAmpPreset)
  .property_readonly("getAmpLimit",&Q250::getAmpLimit)
  .method("start",&Q250::start)
  .method("stop",&Q250::stop)
  ;
  
  
  
  
}
