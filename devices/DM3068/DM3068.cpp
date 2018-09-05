#include "DM3068.h"

#include <string>
#include <thread>
#include <chrono>

foxtrot::devices::DM3068::DM3068(std::shared_ptr< foxtrot::SerialProtocol > proto): 
CmdDevice(proto), _serproto(proto)
{
  proto->Init(nullptr);

}

foxtrot::devices::DM3068::~DM3068()
{

}

string foxtrot::devices::DM3068::getIDNString()
{
	return "not_implemented_yet";
}


string foxtrot::devices::DM3068::cmd(const string& request)
{  
  _serproto->write(request);
  return _serproto->read_until_endl();

}



double foxtrot::devices::DM3068::get4WireResistance()
{
  //set to measurement mode
  auto resstr = cmd(":MEAS:FRES?");
  
  return std::stod(resstr);

}
