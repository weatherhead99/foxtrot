#include <sstream>
#include <foxtrot/Logging.h>
#include "archon.h"
#include "archon_modules.h"
#include "archon_module_generic_bias.h"
#include <rttr/registration>

using foxtrot::devices::archon_module_status;

using namespace foxtrot;
using std::string;

using statptr = optional<vector<double>> archon_module_status::*;
const std::unordered_map<string, std::pair<statptr, statptr>> nmemonic_map =
  {
  {"HC",  {&archon_module_status::HC_Vs, &archon_module_status::HC_Is}},
  {"LC",  {&archon_module_status::LC_Vs, &archon_module_status::LC_Is}},
  {"XVP", {&archon_module_status::XVP_Vs, &archon_module_status::XVP_Is}},
  {"XVN", {&archon_module_status::XVN_Vs, &archon_module_status::XVN_Is}}};


// tuple here first item is can it be enabled/disabled, second item is can it be current set
const std::unordered_map<string, std::tuple<bool,bool>> bias_traits_map =
  {
    {"HC", {true, true}},
    {"LC", {false, false}},
    {"XVP", {true, false}},
    {"XVN", {true, false}}};
   

devices::ArchonGenericBias::ArchonGenericBias(devices::ArchonModule& mod,
					      const string& nmemonic,
					      int numchans, double lowlimit, double highlimit, Logging& lg)
  : _mod(mod), _lg(lg),  _biasnmemonic(nmemonic), _numchans(numchans), _lowlimit(lowlimit), _highlimit(highlimit)
{
  update_traits();
}

void devices::ArchonGenericBias::update_traits()
{
    for( auto& [k, v] : nmemonic_map)
    {
      if(_biasnmemonic.find(k) != std::string::npos)
	{
	  _lg.strm(sl::debug) << "found matching nmemonic";
	  statV = v.first;
	  statI = v.second;

	  //obtain the relevant traits as well;
	  auto [can_enable, can_currentlimit] = bias_traits_map.at(k);
	  _hascurrentlimits = can_currentlimit;
	  _hasenables = can_enable;	  
	  break;
	}
    }
  if(statV == nullptr)
    throw std::logic_error("couldn't find appropriate bias nmemonic");

  _lg.strm(sl::info) << "bias nmemonic is: " << _biasnmemonic;
  _lg.strm(sl::info) << "numchans is: "<< _numchans;

}

void devices::ArchonGenericBias::status(archon_module_status& out, const ssmap& statusmap) const
{
  auto modpos = _mod.info().position;

  std::vector<double> outV;
  std::vector<double> outI;
  outV.reserve(_numchans);
  outI.reserve(_numchans);
  
  for(int channel = 1; channel <= _numchans; channel++)
    {
      _lg.strm(sl::trace) << "channel: " << channel;
      auto Vcmdstr = std::format("MOD{}/{}_V{}", modpos, _biasnmemonic, channel);
      auto Icmdstr = std::format("MOD{}/{}_I{}", modpos, _biasnmemonic, channel);

      _lg.strm(sl::trace) << "Vcmdstr: " << Vcmdstr;
      outV.push_back(std::stod(statusmap.at(Vcmdstr)));
      _lg.strm(sl::trace) << "Icmdstr: " << Icmdstr;
      outI.push_back(std::stod(statusmap.at(Icmdstr)));
      _lg.strm(sl::trace) << "looked up both strings";

    }

  out.*statV = outV;
  out.*statI = outI;
}

std::vector<foxtrot::devices::archon_biasprop> devices::ArchonGenericBias::biases(const ssmap& statusmap) const
{
  _lg.strm(sl::trace) << "reading biases from generic bias module";
  std::vector<archon_biasprop> out;
  out.reserve(_numchans);

  auto modpos = _mod.info().position;
  _lg.strm(sl::trace) << "modpos: " << modpos;
  
  for(decltype(_numchans) i =1; i <= _numchans; i++)
    {
      archon_biasprop prop;
      _lg.strm(sl::trace) << "reading Vmeas";
      auto Vmeasstr = std::format("MOD{}/{}_V{}", modpos,_biasnmemonic, i);
      prop.Vmeas = std::stod(statusmap.at(Vmeasstr));


      _lg.strm(sl::trace) << "reading Imeas";
      auto Imeasstr = std::format("MOD{}/{}_I{}", modpos, _biasnmemonic, i);
      prop.Imeas = std::stod(statusmap.at(Imeasstr));

      _lg.strm(sl::trace) << "reading Vset";
      auto Vsetstr = std::format("{}_V{}",_biasnmemonic, i);

      prop.Vset = _mod.readConfigKey<double>(Vsetstr);
      
      if(_hascurrentlimits)
	{
	  _lg.strm(sl::trace) << "reading Iset";
	  auto Isetstr = std::format("{}_IL{}", _biasnmemonic, i);
	  prop.Iset = _mod.readConfigKey<double>(Isetstr);
	}

      if(_hasenables)
	{
	  _lg.strm(sl::trace) << "reading Enable";
	  auto Enablestr = std::format("{}_ENABLE{}", _biasnmemonic, i);
	  prop.enable = _mod.readConfigKey<bool>(Enablestr);
	}

      prop.name = std::format("{}{}", _biasnmemonic, i);

      _lg.strm(sl::trace) << "reading label";
      auto labelstr = std::format("{}_LABEL{}", _biasnmemonic, i);

      prop.label = _mod.readConfigKeyOpt(labelstr);

      auto orderstr = std::format("{}_ORDER{}", _biasnmemonic, i);
      //order is 0 by default I _think_
      prop.order = _mod.readConfigKey<unsigned>(orderstr, 0u);

      out.push_back(prop);
      
    } 
  return out;
}

const std::string& devices::ArchonGenericBias::nmemonic() const
{
  return _biasnmemonic;
}

void devices::ArchonGenericBias::setLabel(int channel, const string& label)
{
  check_channel_number(channel);
 std::ostringstream oss;
 oss << _biasnmemonic << "_LABEL" << channel;
 _mod.writeConfigKey(oss.str(),label); 
}

string devices::ArchonGenericBias::getLabel(int channel)
{
  check_channel_number(channel);
 std::ostringstream oss;
 oss << _biasnmemonic << "_LABEL" << channel;
 
 return _mod.readConfigKey(oss.str());
  
}

void devices::ArchonGenericBias::setOrder(int channel, int sequence)
{
  check_channel_number(channel);
  auto cmdstr = std::format("{}_ORDER{}", _biasnmemonic, channel);
  
 if(sequence < 0)
   throw std::out_of_range("invalid sequence number < 1");

 _mod.writeConfigKey(cmdstr,std::to_string(sequence));
  
}

int devices::ArchonGenericBias::getOrder(int channel)
{
  check_channel_number(channel);
  std::ostringstream oss;
  oss << _biasnmemonic << "_ORDER" << channel;

  return std::stoi(_mod.readConfigKey(oss.str()));
}

void devices::ArchonGenericBias::setV(int channel, double V)
{
  check_channel_number(channel);
  std::ostringstream oss;
  oss << _biasnmemonic << "_V" << channel;

  std::ostringstream valss;
  valss << std::setprecision(3) << V;
  
  _mod.writeConfigKey(oss.str(),valss.str());
}

double devices::ArchonGenericBias::getV(int channel)
{
  check_channel_number(channel);
   std::ostringstream oss;
  oss << _biasnmemonic << "_V" << channel;
  return std::stod(_mod.readConfigKey(oss.str()));
  
}

void devices::ArchonGenericBias::setEnable(int channel, bool onoff)
{
  check_channel_number(channel);
  std::ostringstream oss;
  oss << _biasnmemonic << "_ENABLE" << channel;
  
  _mod.writeConfigKey(oss.str(),std::to_string((int)onoff));
  
}

bool devices::ArchonGenericBias::getEnable(int channel)
{
  check_channel_number(channel);
  std::ostringstream oss;
  oss <<  _biasnmemonic << "_ENABLE" << channel;
  
  return std::stoi(_mod.readConfigKey(oss.str()));
}

double devices::ArchonGenericBias::measureI(int channel)
{
  check_channel_number(channel);
  auto modpos = _mod.info().position;

  std::ostringstream oss;
  oss << "MOD" << modpos << "/" << _biasnmemonic << "_I" << channel;

  _lg.Debug("measureI request: " + oss.str());

  if(auto ptr = _mod._arch.lock())
    {
      auto str = ptr->getStatus().at(oss.str());
      return std::stod(str);
    }
  else
    throw std::logic_error("couldn't lock archon pointer");

}


double devices::ArchonGenericBias::measureV(int channel)
{
  check_channel_number(channel);
  auto modpos = _mod.info().position;
  auto cmdstr = std::format("MOD{}/{}_V{}", modpos, _biasnmemonic, channel);
  _lg.Debug("measureV request: " + cmdstr);
  if(auto ptr = _mod._arch.lock())
    {
      auto str = ptr->getStatus().at(cmdstr);
      return std::stod(str);
    }
  else
    throw std::logic_error("failed to lock archon pointer");

}



void devices::ArchonGenericBias::check_channel_number(int channel)
{
  if(channel < 1)
  {
    throw std::out_of_range("invalid channel number, <1");
  }
  else if(channel > _numchans)
  {
    throw std::out_of_range("invalid channel number, > num_chans");
  
  }
    
}

void devices::ArchonGenericBias::check_limits(double val)
{
  if(val < _lowlimit)
  {
    throw std::out_of_range("invalid voltage < lowlimit");
  }
  else if (val > _highlimit)
  {
    throw std::out_of_range("invalid voltage > highlimit");
  }

}

void devices::ArchonGenericBias::reconfigure(const string& nmemonic, int numchans, double lowlimit, double highlimit)
{
  _biasnmemonic = nmemonic;
  _numchans = numchans;
  _lowlimit = lowlimit;
  _highlimit = highlimit;

  update_traits();
}


