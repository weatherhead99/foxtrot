#pragma once


#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "archon_module_mapper.hh"
#include <optional>

namespace foxtrot
{
  class Logging;
  
namespace devices
{
  class ArchonModule;
  using std::optional;
  using std::vector;

  class ArchonGenericBias 
  {

    using statptr = optional<vector<double>> archon_module_status::*;
    
      friend class ArchonLVX;
      friend class ArchonHVX;
      friend class ArchonXV;
      
  public:
    
    void setLabel(int channel, const std::string& label);
    std::string getLabel(int channel);
    
    void setOrder(int channel, int sequence);
    int getOrder(int channel);
    
    void setV(int channel, double V);
    double getV(int channel);
    
    void setEnable(int channel, bool onoff);
    bool getEnable(int channel);
    
    [[deprecated]] double measureV(int channel);
    [[deprecated]] double measureI(int channel);
    
    void reconfigure(const std::string& nmemonic, int numchans, double lowlimit, double highlimit);
  
  protected:
    ArchonGenericBias(ArchonModule& mod, const string& biasnmemonic,
      int numchans, double lowlimit, double highlimit, Logging& lg) ;
    
  private:
    void check_channel_number(int channel);
    void check_limits(double val);
    ArchonModule& _mod;
    Logging& _lg; 

    string _biasnmemonic;    
    int _numchans;
    double _lowlimit;
    double _highlimit;

    statptr statV = nullptr;
    statptr statI = nullptr;
    
    
    
  };
  
  
  


}

}
