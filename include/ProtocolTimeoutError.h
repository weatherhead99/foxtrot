#pragma once
#include "ProtocolError.h"
namespace foxtrot
{
  class ProtocolTimeoutError: public ProtocolError
  {
  public:
    ProtocolTimeoutError(const std::string& msg);
    
    
    
  };





}