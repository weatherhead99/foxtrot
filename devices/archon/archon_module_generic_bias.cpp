#include "archon_module_generic_bias.h"
#include <sstream>
#include "archon_modules.h"
#include "Logging.h"


using namespace foxtrot;
using std::string;

devices::ArchonGenericBias::ArchonGenericBias(devices::ArchonModule& mod, const string& nmemonic, 
					      int numchans, double lowlimit, double highlimit, Logging& lg)
: _mod(mod), _biasnmemonic(nmemonic), _numchans(numchans), _lowlimit(lowlimit), _highlimit(highlimit), _lg(lg)
{

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
  std::ostringstream oss;
 oss << _biasnmemonic << "_ORDER" << channel;
 
 if(sequence < 0)
 {
   throw std::out_of_range("invalid sequence number < 1");
 }
 /*else if(sequence > _numchans)
 {
   throw std::out_of_range("invalid sequence number > numchans");
 }
 */  
 _mod.writeConfigKey(oss.str(),std::to_string(sequence));
  
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
  auto modpos = _mod.getmodpos() + 1;
  
  std::ostringstream oss;
  oss << "MOD" << modpos << "/" << _biasnmemonic << "_I" << channel;
  
  _lg.Debug("measureI request: " + oss.str());
  
  auto str = _mod.getArchon().getStatus().at(oss.str());
  return std::stod(str);

}


double devices::ArchonGenericBias::measureV(int channel)
{
  check_channel_number(channel);
  auto modpos = _mod.getmodpos() + 1;
  std::ostringstream oss;
  oss << "MOD" << modpos << "/" << _biasnmemonic << "_V" << channel;
  
  _lg.Debug("measureV request: " + oss.str());
  
  auto str = _mod.getArchon().getStatus().at(oss.str());
  return std::stod(str);

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


