#pragma once
#include <memory>

namespace foxtrot
{
  namespace devices
  {
    //fwd declare
    class archon;
    class ArchonModule;

    
    enum class archon_module_types : short unsigned
    {
        None  = 0,
        Driver = 1,
        AD = 2,
        LVBias = 3,
        HVBias = 4,
        Heater = 5,
        HS = 7,
        HVXBias = 8,
        LVXBias = 9,
        LVDS = 10,
	HeaterX = 11,
	XVBias = 12,
	ADF = 13,
	ADX = 14,
	ADLN = 15,
	DriverX = 16,
	ADM = 17
    };

    std::unique_ptr<ArchonModule> make_module(archon& arch, int modpos, archon_module_types modtp);

    std::string get_module_name(archon_module_types modtp);

  }
}
