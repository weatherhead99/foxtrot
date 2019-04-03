#pragma once
#include "Error.h"

namespace foxtrot 
{
    class FOXTROT_EXPORT ServerError : public Error 
    {
        public:
        ServerError(const std::string& msg);
        
    };
}
