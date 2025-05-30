#pragma once
#include <foxtrot/Logging.h>

#include "archon_modules.h"
#include "archon_module_lvxbias.h"
#include "archon_GPIO.h"

namespace foxtrot
{
    namespace devices
    {
        class archon;
        
        class ArchonXV : public ArchonLVX
        {
	  friend class ArchonModule;
          RTTR_ENABLE(ArchonLVX)
        public:
            static std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos);
            
            const std::string getTypeName() const override;
	    void update_variables() override;
	    
	    const string getDeviceTypeName() const override;
                        
            
        private:
            ArchonXV(archon& arch, int modpos);
            
            
            
            
        };


}
}
