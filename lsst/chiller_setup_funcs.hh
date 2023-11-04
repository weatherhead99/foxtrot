#pragma once
#include <foxtrot/DeviceHarness.h>
#include <foxtrot/parameter_defs.hh>

namespace foxtrot
{
  class Logging;
  class DeviceHarness;
  
}


using mapofparametersets = std::map<std::string, foxtrot::parameterset>;


void setup_chiller(foxtrot::Logging& lg,
		   foxtrot::DeviceHarness& harness,
		   const mapofparametersets* const params);

  
