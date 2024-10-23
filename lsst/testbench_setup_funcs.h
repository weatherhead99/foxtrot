#pragma once 

#include <map>
#include <foxtrot/DeviceHarness.h>
#include <archon_modules.h>



using modulesmap = std::map<int, devices::ArchonModule&>;

template <typename T> std::unique_ptr<foxtrot::Device, void(*)(foxtrot::Device*)>
get_ptr_for_harness( const T* ptr)
{
  return std::unique_ptr<foxtrot::Device,void(*)(foxtrot::Device*)>
    (static_cast<foxtrot::Device*>(
      const_cast<T*>(ptr)),[](foxtrot::Device*){});
}



void setup_heaterX(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg);
void setup_hvxbias(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg);
void setup_lvxbias(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset);
void setup_xvbias(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset);
void setup_clockdriver(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset);
void setup_clockdriver2(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset);
void setup_ADCs(const modulesmap& modules, foxtrot::DeviceHarness& harness, foxtrot::Logging& lg, bool archon_reset);
