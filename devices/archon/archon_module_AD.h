#pragma once
#include "archon_modules.h"
#include <sstream>

namespace foxtrot
{
  namespace devices
  {
    class archon;
    
    class ArchonAD: public ArchonModule
    {
      RTTR_ENABLE(ArchonModule)
      
    public:
      static std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos);
      
      virtual const string getTypeName() const override;
      virtual void update_variables() override;
      
      void setClamp(int channel, double val);
      double getClamp(int channel);
      
      void setPreampGain(bool hgain);
      bool getPreampGain();
      
    private:
      ArchonAD(archon& arch, short unsigned int modpos);
      std::ostringstream _oss;
      
      inline static void checkChannelNum(int channel)
      {
	if(channel < 1 || channel > 4)
	{
	  throw std::out_of_range("invalid AD channel number");
	}
	
      };
      
      
    };
    
  }
  
}
