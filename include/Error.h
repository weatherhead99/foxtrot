#pragma once

#include <stdexcept>

#include <string>
#include "foxtrot_export.h"

namespace foxtrot
{
  class FOXTROT_EXPORT Error : public std::runtime_error
  {
  public:
    Error(const std::string& msg);
    
    
  };
  
  
  
};