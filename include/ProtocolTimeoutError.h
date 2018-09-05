#pragma once
#include "ProtocolError.h"
namespace foxtrot
{
  class FOXTROT_EXPORT ProtocolTimeoutError: public ProtocolError
  {
  public:
    ProtocolTimeoutError(const std::string& msg);
    
    
    
  };





}