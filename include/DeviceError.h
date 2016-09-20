#pragma once
#include "Error.h"

namespace foxtrot
{

  class DeviceError : public Error
  {
  public:
   DeviceError(const std::string& msg); 
    
  };



}