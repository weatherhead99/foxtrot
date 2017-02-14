#include "ExperimentalSetup.h"
#include <dlfcn.h>
#include "Error.h"

using namespace foxtrot;

foxtrot::ExperimentalSetup::ExperimentalSetup(const std::string& setupfile, foxtrot::DeviceHarness& harness)
: _harness(harness)
{
  _dl = dlopen(setupfile.c_str(), RTLD_LAZY);
  if(_dl == nullptr)
  {    
    throw std::runtime_error(dlerror());
  }
  
  //clear dlerror
  dlerror();
  
  auto sym = dlsym(_dl,"setup");
  
  if(sym == nullptr)
  {
   throw std::runtime_error("couldn't find setup() symbol in setup file");
  }
  
  auto setup_fun = reinterpret_cast<int(*)(foxtrot::DeviceHarness&)>(sym);
  
  setup_fun(harness);
  
    
};

foxtrot::ExperimentalSetup::~ExperimentalSetup()
{
    dlclose(_dl);
}

