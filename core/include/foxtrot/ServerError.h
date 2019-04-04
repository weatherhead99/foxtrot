#pragma once
#include <foxtrot/Error.h>

namespace foxtrot 
{
    class FOXTROT_CORE_EXPORT ServerError : public Error 
    {
        public:
        ServerError(const std::string& msg);
        
    };
}
