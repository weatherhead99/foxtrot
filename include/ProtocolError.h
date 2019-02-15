#pragma once

#include "Error.h"

namespace foxtrot
{
 class Logging;
    
 class FOXTROT_EXPORT ProtocolError : public Error 
 {
 public:
    ProtocolError(const std::string& msg);
    ProtocolError(const std::string& msg, Logging& lg);
   
     
 };
  
  
}
