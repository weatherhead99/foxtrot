#pragma once

#include <stdexcept>

#include <string>

namespace foxtrot
{
  class Error : public std::runtime_error
  {
  public:
    Error(const std::string& msg);
    
    
  };
  
  
  
};