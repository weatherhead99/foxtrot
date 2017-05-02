#pragma once	
#include "archon_modules.h"
#include "archon_module_generic_bias.h"
#include <rttr/registration>
#include "archon_GPIO.h"

namespace foxtrot
{
    namespace devices
    {
        class archon;
        
        class ArchonLVX : public ArchonModule, public archonGPIO
        {
        RTTR_ENABLE(ArchonModule,archonGPIO)
        public:
	    
	    static std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos);
	    
	    virtual const string getTypeName() const override;
	    
	    virtual void update_variables() override;
	  
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
            ArchonGenericBias _lcbias;
            ArchonGenericBias _hcbias;
            ArchonLVX(archon& arch, short unsigned int modpos);
	    
            
        private:
            
	    std::array<bool,8> _GPIO;
            
        };
        
            
        
    }
    
}
