#include "archon_defs.hh"
#include <rttr/registration>
#include <sys/types.h>
#include <foxtrot/ft_union_helper.hh>

RTTR_REGISTRATION
{
  using namespace rttr;
  using foxtrot::devices::archon_gpio_source;
  
   registration::enumeration<archon_gpio_source>("foxtrot::devices::gpio_source")
   (
    value("low", archon_gpio_source::low),
    value("high", archon_gpio_source::high),
    value("clocked", archon_gpio_source::clocked),
    value("VCPU", archon_gpio_source::VCPU)
    );

   using foxtrot::devices::archon_driverprop;
   registration::class_<archon_driverprop>("foxtrot::devices::archon_driverprop")
     .constructor()(policy::ctor::as_object)
     .property("chan", &archon_driverprop::chan)
     .property("label", &archon_driverprop::label)
     .property("slew_slow", &archon_driverprop::slew_slow)
     .property("slew_fast", &archon_driverprop::slew_fast)
     .property("source", &archon_driverprop::source)
     .property("enable", &archon_driverprop::enable);

   using foxtrot::devices::archon_biasprop;
   registration::class_<archon_biasprop>("foxtrot::devices::archon_biasprop")
     .constructor()(policy::ctor::as_object)
     .property("Vset", &archon_biasprop::Vset)
     .property("label", &archon_biasprop::label)
     .property("name", &archon_biasprop::name)
     .property("Iset", &archon_biasprop::Iset)
     .property("enable", &archon_biasprop::enable)
     .property("Imeas", &archon_biasprop::Imeas)
     .property("Vmeas", &archon_biasprop::Vmeas)
     .property("order", &archon_biasprop::order)
     ;

   using foxtrot::devices::ArchonModuleProp;
   foxtrot::register_union<ArchonModuleProp>();
   
}
