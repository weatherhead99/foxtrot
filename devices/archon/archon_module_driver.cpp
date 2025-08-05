#include "archon_module_driver.h"
#include "archon.h"
#include <rttr/detail/registration/registration_impl.h>

using foxtrot::devices::ArchonModule ;
using foxtrot::devices::ArchonDriverBase;
using foxtrot::devices::ArchonDriver;
using foxtrot::devices::ArchonDriverX;
using foxtrot::devices::archon;

ArchonDriver::ArchonDriver(std::weak_ptr<archon>& arch, const archon_module_info& modinfo)
  : ArchonDriverBase<detail::eight_t>(arch, modinfo) {}

ArchonDriver::~ArchonDriver() {}

const string ArchonDriver::getTypeName() const
{
  return "Driver";
}

ArchonDriverX::ArchonDriverX(std::weak_ptr<archon>& arch, const archon_module_info& modinfo)
  : ArchonDriverBase<detail::twelve_t>(arch, modinfo) {}

ArchonDriverX::~ArchonDriverX() {}

const string ArchonDriverX::getTypeName() const
{
  return "DriverX";
}


template <typename T> void add_shared_base_meths(auto &&binder) {
  using namespace rttr;
  using foxtrot::devices::archon_driverprop;
    binder.method("setLabel", &T::setLabel)(parameter_names("channel", "label"))
        .method("getLabel", &T::getLabel)(parameter_names("channel"))
        .method("setSlowSlewRate",
                &T::setSlowSlewRate)(parameter_names("channel", "val"))
        .method("getSlowSlewRate",
                &T::getSlowSlewRate)(parameter_names("channel"))
        .method("getFastSlewRate",
                &T::getFastSlewRate)(parameter_names("channel"))
        .method("setFastSlewRate",
                &T::setFastSlewRate)(parameter_names("channel", "val"))
        .method("setEnable", &T::setEnable)(parameter_names("channel", "onoff"))
        .method("getEnable", &T::getEnable)(parameter_names("channel"))
      .method("clocks", rttr::select_overload<std::vector<archon_driverprop>()>(&T::clocks));
  }


RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::ArchonDriverBase;
  using foxtrot::devices::ArchonDriver;
  using foxtrot::devices::ArchonDriverX;


  add_shared_base_meths<ArchonDriver>(registration::class_<ArchonDriver>("foxtrot::devices::ArchonDriver"));

  add_shared_base_meths<ArchonDriverX>(
				       registration::class_<ArchonDriverX>("foxtrot::devices::ArchonDriverX"));
  
}



// RTTR_REGISTRATION
// {
//     using namespace rttr;
//     using foxtrot::devices::ArchonDriver;
    
//     registration::class_<ArchonDriver>("foxtrot::devices::ArchonDriver")
//     .method("setLabel", &ArchonDriver::setLabel)
//     (parameter_names("channel","label"))
//     .method("getLabel", &ArchonDriver::getLabel)
//     (parameter_names("channel"))
//     .method("setSlowSlewRate", &ArchonDriver::setSlowSlewRate)
//     (parameter_names("channel","val"))
//     .method("getSlowSlewRate", &ArchonDriver::getSlowSlewRate)
//     (parameter_names("channel"))
//     .method("getFastSlewRate",&ArchonDriver::getFastSlewRate)
//     (parameter_names("channel"))
//     .method("setFastSlewRate",&ArchonDriver::setFastSlewRate)
//     (parameter_names("channel","val"))
//     .method("setEnable",&ArchonDriver::setEnable)
//     (parameter_names("channel","onoff"))
//     .method("getEnable",&ArchonDriver::getEnable)
//     (parameter_names("channel"))
//     ;
    
// }

