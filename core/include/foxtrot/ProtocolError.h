#pragma once

#include <foxtrot/Error.h>

namespace foxtrot
{
 class Logging;
    
 class FOXTROT_CORE_EXPORT ProtocolError : public Error 
 {
 public:
    ProtocolError(const std::string& msg);
    ProtocolError(const std::string& msg, Logging& lg);
   
     
 };
  
  
}
