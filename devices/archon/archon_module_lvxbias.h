#pragma once
#include "archon_modules.h"
#include "archon_module_generic_bias.h"

namespace foxtrot
{
    namespace devices
    {
        class archon;
        
        class ArchonLVX : public ArchonModule
        {
        public:
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
            
        protected:
            
        private:
            ArchonLVX(archon& arch, short unsigned int modpos);
            
            ArchonGenericBias _lcbias;
            ArchonGenericBias _hcbias;
            
        };
        
            
        
    }
    
}
