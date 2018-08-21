#pragma once

#include "Error.h"

namespace foxtrot
{
 class FOXTROT_EXPORT ProtocolError : public Error 
 {
 public:
    ProtocolError(const std::string& msg);
   
     
 };
  
  
}