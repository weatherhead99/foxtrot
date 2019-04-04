
#pragma once

#include <stdexcept>
#include <string>

#include <foxtrot/foxtrot_core_export.h>

namespace foxtrot
{
  class Logging;
    
    
  class FOXTROT_CORE_EXPORT Error : public std::runtime_error
  {
  public:
    Error(const std::string& msg);
    Error(const std::string& msg, Logging& lg);
    
    
  };
  
  
  
};
