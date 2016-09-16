#pragma once
#include "CmdDevice.h"

#include <type_traits>
#include <memory>
#include <typeinfo>

#include <functional>


namespace foxtrot
{
  
  template <typename baseT> 
  class expose_cmd : public baseT
  {
    
    static_assert(std::is_base_of<CmdDevice, baseT>::value, "require CmdDevice as base class");
    
  public:
    template <typename protoT> expose_cmd(protoT proto) : baseT(proto)
    {
      
    };
    
    
    virtual std::string cmd(const std::string& request) override
    {
     return baseT::cmd(request);
    }
  protected:
    
    
    
    
    
  };
  
  
  
  
} //namespace foxtrot