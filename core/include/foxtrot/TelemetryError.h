#pragma once

#include <foxtrot/Error.h>

namespace foxtrot {

    class TelemetryError : public Error
    {
        public:
        TelemetryError(const std::string& msg);
      
        
    };
    

}
