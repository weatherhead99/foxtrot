#pragma once
#include "archon_modules.h"
#include "archon_module_lvxbias.h"
#include <rttr/registration>
#include "Logging.h"

namespace foxtrot
{
  namespace devices
  {
    class archon;
    class ArchonHVX : public ArchonLVX
    {
      RTTR_ENABLE(ArchonLVX)
      
    public:
      static std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos);
      
    const string getTypeName() const override;
    void update_variables() override;
    const string getDeviceTypeName() const override;
      
    
    private:
      ArchonHVX(archon& arch, short unsigned int modpos);
      
      
    };
    
  }
  
}
  
  
