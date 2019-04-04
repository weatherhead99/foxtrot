#pragma once

#include <foxtrot/Error.h>

namespace foxtrot
{
  class FOXTROT_CORE_EXPORT ContentionError : public Error 
  {
  public:
    ContentionError(const std::string& msg);
    
  };

};
