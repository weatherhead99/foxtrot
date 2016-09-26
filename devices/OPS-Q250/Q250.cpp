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

double devices::Q250::getVolts()
{
  auto repl = cmd("VOLTS?");
  return std::stod(repl);
}

double devices::Q250::getWatts()
{
  auto repl = cmd("WATTS?");
  return std::stod(repl);
}
