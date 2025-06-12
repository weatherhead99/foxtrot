#include "archon_module_mapper.hh"
#include <map>
#include <boost/hana.hpp>
#include <string>

#include "archon_modules.h"
#include "archon_module_driver.h"
#include "archon_module_AD.h"
#include "archon_module_lvxbias.h"
#include "archon_module_LVDS.hh"
#include "archon_module_hvxbias.h"
#include "archon_module_heaterx.h"
#include "archon_module_xvbias.h"

#include <rttr/registration>


using foxtrot::devices::archon_module_types;
using std::unique_ptr;
using std::make_unique;

using namespace foxtrot::devices;


const std::map<archon_module_types, std::string> module_type_names = {
    {archon_module_types::Driver, "Driver"},
    {archon_module_types::AD, "AD"},
    {archon_module_types::LVBias, "LVBias"},
    {archon_module_types::HVBias, "HVBias"},
    {archon_module_types::Heater, "Heater"},
    {archon_module_types::HS, "HS"},
    {archon_module_types::HVXBias, "HVXBias"},
    {archon_module_types::LVXBias, "LVXBias"},
    {archon_module_types::LVDS, "LVDS"},
    {archon_module_types::HeaterX, "HeaterX"},
    {archon_module_types::XVBias, "XVBias"},
    {archon_module_types::ADF, "ADF"},
    {archon_module_types::ADX, "ADX"},
    {archon_module_types::ADLN, "ADLN"},
    {archon_module_types::DriverX, "DriverX"},
    {archon_module_types::ADM, "ADM"}};


namespace h = boost::hana;

constexpr short unsigned amtval(archon_module_types tp)
{
  return static_cast<short unsigned>(tp);
}

using foxtrot::devices::ArchonDriver;
using foxtrot::devices::ArchonAD;

using amt = archon_module_types;


//only adding for the ones we've actually implemented!
auto modtpmap = h::make_map(
			    h::make_pair(h::int_c<amtval(amt::Driver)>, h::type_c<ArchonDriver>),
			    h::make_pair(h::int_c<amtval(amt::AD)>, h::type_c<ArchonAD>),
			    //LVBias: unimplemented!
			    //HVBias: unimplemented!
			    //Heater: unimplemented!
			    //HS: unimplemented!
			    h::make_pair(h::int_c<amtval(amt::HVXBias)>, h::type_c<ArchonHVX>),
			    h::make_pair(h::int_c<amtval(amt::LVXBias)>, h::type_c<ArchonLVX>),
			    h::make_pair(h::int_c<amtval(amt::LVDS)>, h::type_c<ArchonLVDS>),
			    h::make_pair(h::int_c<amtval(amt::HeaterX)>, h::type_c<ArchonHeaterX>),
			    h::make_pair(h::int_c<amtval(amt::XVBias)>, h::type_c<ArchonXV>),
			    //ADF: unimplemented!
			    //ADX: unimplemented!
			    //ADLN: unimplemented!
			    h::make_pair(h::int_c<amtval(amt::DriverX)>, h::type_c<ArchonDriverX>),
			    h::make_pair(h::int_c<amtval(amt::ADM)>, h::type_c<ArchonADM>)
			    );




unique_ptr<ArchonModule> foxtrot::devices::make_module(std::weak_ptr<archon>&& arch, const archon_module_info& inf)
{
  std::unique_ptr<ArchonModule> modptr = nullptr;
  h::for_each(modtpmap, [&arch, &inf,  &modptr] (auto v)
  {
    using ModType = typename decltype(+h::second(v))::type;
    const short unsigned key = h::value(h::first(v));
    if(modptr != nullptr)
      return;
    if(static_cast<short unsigned>(inf.type) == key)
      {
	modptr = ArchonModule::constructModule<ModType>(arch, inf);
      }

       });

  return modptr;
}


std::string foxtrot::devices::get_module_name(archon_module_types modtp)
{
  if(not module_type_names.contains(modtp))
    return std::format("unknown module, type number: {}", static_cast<short unsigned>(modtp));
  return module_type_names.at(modtp);
}

RTTR_REGISTRATION{

  using namespace rttr;
  using foxtrot::devices::archon_module_types;

  registration::enumeration<archon_module_types>("foxtrot::devices::archon_module_types")
    (value("None", archon_module_types::None),
     value("Driver", archon_module_types::Driver),
     value("AD", archon_module_types::AD),
     value("LVBias", archon_module_types::LVBias),
     value("HVBias", archon_module_types::HVBias),
     value("Heater", archon_module_types::Heater),
     value("HS", archon_module_types::HS),
     value("HVXBias", archon_module_types::HVXBias),
     value("LVXBias", archon_module_types::LVXBias),
     value("LVDS", archon_module_types::LVDS),
     value("HeaterX", archon_module_types::HeaterX),
     value("XVBias", archon_module_types::XVBias),
     value("ADF", archon_module_types::ADF),
     value("ADX", archon_module_types::ADX),
     value("ADLN", archon_module_types::ADLN),
     value("DriverX",  archon_module_types::DriverX),
     value("ADM", archon_module_types::ADM));
     

}
