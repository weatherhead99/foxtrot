#pragma once

#include <foxtrot/Error.h>

namespace foxtrot {
    
    class FOXTROT_CORE_EXPORT ReflectionError: public Error 
    {
    public:
        ReflectionError(const std::string& msg);
        
    };
    
}
