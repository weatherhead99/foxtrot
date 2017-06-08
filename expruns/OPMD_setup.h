#pragma once

#include "SerialPort.h"
#include "scsiserial.h"
#include "BulkUSB.h"

#include "cornerstone_260/cornerstone260.h"
#include "OPS-Q250/Q250.h"
#include "newport_2936R/newport2936R.h"

#include <memory>

using namespace foxtrot;

class OPMD_setup
{
public:
  OPMD_setup();
  
  devices::cornerstone260& getmonoch();
  devices::newport2936R& getOPM();
  devices::Q250 getPSU();
  
  
private:
  std::shared_ptr<protocols::SerialPort> _cornerstone_sport;
  std::shared_ptr<protocols::scsiserial> _psu_proto;
  std::shared_ptr<protocols::BulkUSB> _OPM_proto;
  
  devices::cornerstone260 _monoch;
  devices::newport2936R _opm;
  devices::Q250 _psu;
  
  
};