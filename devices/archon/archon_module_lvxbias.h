#pragma once		
#include <rttr/registration>
#include <foxtrot/Logging.h>

#include "archon_defs.hh"
#include "archon_modules.h"
#include "archon_module_generic_bias.h"
#include "archon_GPIO.h"

namespace foxtrot
{
    namespace devices
    {
        class archon;
        
        class ArchonLVX : public ArchonModule, public archonGPIO
        {
	  friend class ArchonModule;
        RTTR_ENABLE(ArchonModule,archonGPIO)
        public:

	std::vector<archon_biasprop> biases(const ssmap& statusmap) const;
          std::vector<archon_biasprop> biases();

	  std::vector<ArchonModuleProp> props(const ssmap& statusmap) const override;
	  
	  void status(archon_module_status& out, const ssmap& statusmap) const override;
	    
	    const string getTypeName() const override;
	    const string getDeviceTypeName() const override;
	  
            void setLabel(bool HC, int channel, const std::string& label);
            std::string getLabel(bool HC, int channel);
            
            void setOrder(bool HC, int channel, int sequence);
            int getOrder(bool HC, int channel);
            
            void setV(bool HC, int channel, double V);
            double getV(bool HC, int channel);
            
            void setEnable(bool HC, int channel, bool onoff);
            bool getEnable(bool HC, int channel);
            
            void setLimit(int channel, int limit_mA);
            int getLimit(int channel);
            
            //TODO: GPIO
            double measureV(bool HC, int channel);
	    double measureI(bool HC, int channel);
	    
	    
        protected:
            Logging _lg; 
            ArchonGenericBias _lcbias;
            ArchonGenericBias _hcbias;
            ArchonLVX(std::weak_ptr<archon>& arch, const archon_module_info& modinfo);
	    

            
        private:
	    std::array<bool,8> _GPIO;
            
        };
        
            
        
    }
    
}
