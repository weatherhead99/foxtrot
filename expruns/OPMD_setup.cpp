#include "OPMD_setup.h"
#include <map>

const foxtrot::parameterset psu_params
{
  {"devnode", "/dev/sdb"},
  {"timeout", 2000u}
};

const foxtrot::parameterset monoch_params
{
  {"port", "/dev/ttyS0"}
};



OPMD_setup::OPMD_setup()
: _cornerstone_sport{ std::make_shared<protocols::SerialPort>(&monoch_params)},
 _psu_proto{ std::make_shared<protocols::scsiserial>(&psu_params)},
 _OPM_proto{ std::make_shared<protocols::BulkUSB>(nullptr)},
 _monoch(_cornerstone_sport),
 _opm(_OPM_proto),
 _psu(_psu_proto)
{
  
  

}


devices::cornerstone260& OPMD_setup::getmonoch()
{
  return _monoch;

}

devices::newport2936R& OPMD_setup::getOPM()
{
  return _opm;
}


devices::Q250 OPMD_setup::getPSU()
{
  return _psu;

}

