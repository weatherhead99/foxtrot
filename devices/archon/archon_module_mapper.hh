#pragma once
#include <memory>
#include <string>

namespace foxtrot
{
  namespace devices
  {
    //fwd declare
    class archon;
    class ArchonModule;

    using std::string;
    
    
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
    
    struct archon_module_info
    {
      unsigned position;
      archon_module_types type;
      unsigned revision;
      string version;
      unsigned long long module_id;
    };

    std::unique_ptr<ArchonModule> make_module(std::weak_ptr<archon>&& arch, const archon_module_info& inf);

    std::string get_module_name(archon_module_types modtp);

  }
}
