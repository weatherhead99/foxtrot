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
  
    
};

foxtrot::ExperimentalSetup::~ExperimentalSetup()
{
    dlclose(_dl);
}

