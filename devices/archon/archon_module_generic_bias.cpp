#include <sstream>
#include <foxtrot/Logging.h>
#include "archon.h"
#include "archon_modules.h"
#include "archon_module_generic_bias.h"


using foxtrot::devices::archon_module_status;

using namespace foxtrot;
using std::string;

using statptr = optional<vector<double>> archon_module_status::*;
const std::map<string, std::pair<statptr, statptr>> nmemonic_map =
  {
  {"HC",  {&archon_module_status::HC_Vs, &archon_module_status::HC_Is}},
  {"LC",  {&archon_module_status::LC_Vs, &archon_module_status::LC_Is}},
  {"XVP", {&archon_module_status::XVP_Vs, &archon_module_status::XVP_Is}},
  {"XVN", {&archon_module_status::XVN_Vs, &archon_module_status::XVN_Is}}};

devices::ArchonGenericBias::ArchonGenericBias(devices::ArchonModule& mod,
					      const string& nmemonic,
					      int numchans, double lowlimit, double highlimit, Logging& lg)
  : _mod(mod), _lg(lg),  _biasnmemonic(nmemonic), _numchans(numchans), _lowlimit(lowlimit), _highlimit(highlimit)
{
  for( auto& [k, v] : nmemonic_map)
    {
      if(nmemonic.find(k) != std::string::npos)
	{
	  lg.strm(sl::debug) << "found matching nmemonic";
	  statV = v.first;
	  statI = v.second;
	}
    }
  if(statV == nullptr)
    throw std::logic_error("couldn't find appropriate bias nmemonic");
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
  

}


