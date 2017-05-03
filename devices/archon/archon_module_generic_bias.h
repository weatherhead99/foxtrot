#pragma once


#include <memory>
#include <string>
#include <tuple>

namespace foxtrot
{
  class Logging;
  
namespace devices
{
  class ArchonModule;
    
  class ArchonGenericBias 
  {
      
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
    
    double measureV(int channel);
    double measureI(int channel);
    
    void reconfigure(const std::string& nmemonic, int numchans, double lowlimit, double highlimit);
  
  protected:
    ArchonGenericBias(ArchonModule& mod, const std::string& nmemonic,
      int numchans, double lowlimit, double highlimit, Logging& lg) ;
    
  private:
    void check_channel_number(int channel);
    void check_limits(double val);
    
    ArchonModule& _mod;
    std::string _biasnmemonic;
    Logging& _lg; 
    
    int _numchans;
    double _lowlimit;
    double _highlimit;
    
    
    
  };
  
  
  


}

}
