#pragma once

#include "Error.h"

namespace foxtrot
{
  class FOXTROT_EXPORT ContentionError : public Error 
  {
  public:
    ContentionError(const std::string& msg);
    
  };

};