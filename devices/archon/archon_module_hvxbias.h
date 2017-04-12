#pragma once
#include "archon_modules.h"
#include "archon_module_generic_bias.h"
#include <rttr/registration>


namespace foxtrot
{
  namespace devices
  {
    class archon;
    class ArchonHVX : public ArchonModule 
    {
      RTTR_ENABLE(ArchonModule)
      
    public:
      static std::unique_ptr<ArchonModule> constructModule(archon& arch, int modpos);
      
    virtual const string getTypeName() const override;
    virtual void update_variables() override;
    
    	  
    
    
    
      
    };
    
  }
  
}
  
  
