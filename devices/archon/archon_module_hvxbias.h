#pragma once
#include "archon_modules.h"
#include "archon_module_lvxbias.h"
#include <rttr/registration>


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
      
    virtual const string getTypeName() const override;
    virtual void update_variables() override;
      virtual const string getDeviceTypeName() const;
      
    
    private:
      ArchonHVX(archon& arch, short unsigned int modpos);
      ArchonGenericBias _lcbias;
      ArchonGenericBias _hcbias;
    
    
    
      
    };
    
  }
  
}
  
  
