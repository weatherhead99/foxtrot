#pragma once

#include "archon.h"
#include <rttr/registration>

namespace foxtrot {

    namespace devices {
        class archonraw : public archon
        {
            RTTR_ENABLE(archon)
        public:
            std::string rawcmd(const std::string& request);
            
            
            
        };
            
        
    }
    


}
