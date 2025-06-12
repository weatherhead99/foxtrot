#pragma once
#include <rttr/registration>

#include <foxtrot/Logging.h>

#include "archon_modules.h"
#include "archon_module_lvxbias.h"

namespace foxtrot
{
  namespace devices
  {
    class archon;
    class ArchonHVX : public ArchonLVX
    {
      RTTR_ENABLE(ArchonLVX)
      
    public:
      friend class ArchonModule;
    const string getTypeName() const override;
    const string getDeviceTypeName() const override;
      
    
    private:
      ArchonHVX(std::weak_ptr<archon>& arch, const archon_module_info& modinf);
      
      
    };
    
  }
  
}
  
  
