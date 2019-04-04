#pragma once
#include <foxtrot/Error.h>

namespace foxtrot
{

  class FOXTROT_CORE_EXPORT DeviceError : public Error
  {
  public:
   DeviceError(const std::string& msg); 
    
  };



}
