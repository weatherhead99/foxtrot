#pragma once
#include "archon_modules.h"

namespace foxtrot
{
 namespace devices
 {
     class archon;
     
  class ArchonDriver : public ArchonModule 
  {
    RTTR_ENABLE(ArchonModule)
    
  public:
      static std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos);
      
      virtual const string getTypeName() const override;
      virtual void update_variables() override;
      
      void setLabel(int channel, const std::string& val);
      std::string getLabel(int channel);
      
      void setSlowSlewRate(int channel, double val);
      double getSlowSlewRate(int channel);
      
      void setFastSlewRate(int channel, double val);
      double getFastSlewRate(int channel);
      
      void setEnable(int channel, bool onoff);
      bool getEnable(int channel);
      
  private:
      ArchonDriver(archon& arch, short unsigned int modpos);
      std::ostringstream _oss;
      
      inline static void checkChannel(int channel)
      {
          if(channel < 1 || channel > 8)
          {
              throw std::out_of_range("invalid Driver channel number");
          };
      };
      
      
  };
     
 }
    
    
    
}
