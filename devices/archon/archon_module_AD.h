#pragma once
#include "archon_module_mapper.hh"
#include "archon_modules.h"
#include <sstream>

namespace foxtrot
{
  namespace devices
  {
    class archon;
    
    class ArchonAD: public ArchonModule
    {
      friend class ArchonModule;
      RTTR_ENABLE(ArchonModule)
    public:
      
      virtual const string getTypeName() const override;

      void setClamp(int channel, double val);
      double getClamp(int channel);
      
      void setPreampGain(bool hgain);
      bool getPreampGain();
      
    private:
      ArchonAD(std::weak_ptr<archon>& arch, const archon_module_info& modinf);
      std::ostringstream _oss;
      
      inline static void checkChannelNum(int channel)
      {
	if(channel < 1 || channel > 4)
	{
	  throw std::out_of_range("invalid AD channel number");
	}
	
      };
      
      
    };

    class ArchonADM : public ArchonModule
    {
      //I _think_ ADM module has no configuration options at all!
      friend class ArchonModule;
      RTTR_ENABLE(ArchonModule)
    public:
      virtual const string getTypeName() const override;
      ArchonADM(std::weak_ptr<archon>& arch, const archon_module_info& modinf);
      
    };
    
  }
  
}
