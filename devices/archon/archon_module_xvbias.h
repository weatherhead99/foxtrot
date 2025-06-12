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
	  const std::string getTypeName() const override;
	  const string getDeviceTypeName() const override;
                        
            
        private:
	  ArchonXV(std::weak_ptr<archon>& arch, const archon_module_info& modpos);
            
            
            
            
        };


}
}
