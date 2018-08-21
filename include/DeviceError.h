#pragma once
#include "Error.h"

namespace foxtrot
{

  class FOXTROT_EXPORT DeviceError : public Error
  {
  public:
   DeviceError(const std::string& msg); 
    
  };



}