#pragma once


#include <memory>
#include <string>
#include <tuple>

namespace foxtrot
{
namespace devices
{
  class ArchonModule;
    
  class ArchonGenericBias 
  {
      
      friend class ArchonLVX;
      friend class ArchonHVX;
      
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
    
    
  
  protected:
    ArchonGenericBias(ArchonModule& mod, const std::string& nmemonic,
      int numchans, double lowlimit, double highlimit) ;
    
  private:
    void check_channel_number(int channel);
    void check_limits(double val);
    
    ArchonModule& _mod;
    std::string _biasnmemonic;
     
    int _numchans;
    double _lowlimit;
    double _highlimit;
    
    
    
  };
  
  
  


}

}
