#pragma once
#include <foxtrot/ProtocolError.h>
namespace foxtrot
{
  class FOXTROT_CORE_EXPORT ProtocolTimeoutError: public ProtocolError
  {
  public:
    ProtocolTimeoutError(const std::string& msg);
    
    
    
  };





}
