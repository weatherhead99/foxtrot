#include "archon_module_mapper.hh"
#include <map>
#include <boost/hana.hpp>
#include <string>

#include "archon_modules.h"
#include "archon_module_driver.h"
#include "archon_module_AD.h"
#include "archon_module_lvxbias.h"
#include "archon_module_hvxbias.h"
#include "archon_module_heaterx.h"
#include "archon_module_xvbias.h"

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
			    //LVDS: unimplemented!
			    h::make_pair(h::int_c<amtval(amt::HeaterX)>, h::type_c<ArchonHeaterX>),
			    h::make_pair(h::int_c<amtval(amt::XVBias)>, h::type_c<ArchonXV>),
			    //ADF: unimplemented!
			    //ADX: unimplemented!
			    //ADLN: unimplemented!
			    h::make_pair(h::int_c<amtval(amt::DriverX)>, h::type_c<ArchonDriverX>)
			    //ADM: unimplemented!
			    );




unique_ptr<ArchonModule> foxtrot::devices::make_module(archon& arch, int modpos, archon_module_types modtp)
{
  std::unique_ptr<ArchonModule> modptr = nullptr;
  h::for_each(modtpmap, [&arch, modpos, modtp, &modptr] (auto v)
  {
    using ModType = typename decltype(+h::second(v))::type;
    const short unsigned key = h::value(h::first(v));
    if(modptr != nullptr)
      return;
    if(static_cast<short unsigned>(modtp) == key)
      {
	//found a match!
	modptr = ArchonModule::constructModule<ModType>(arch, modpos);
      }

       });


  return modptr;
}


std::string foxtrot::devices::get_module_name(archon_module_types modtp)
{
  return module_type_names.at(modtp);
}
