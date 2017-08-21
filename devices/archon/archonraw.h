#pragma once

#include "archon.h"
#include <rttr/registration>

namespace foxtrot {

    namespace devices {
        class archonraw : public archon
        {
            RTTR_ENABLE(archon)
        public:
	    archonraw(std::shared_ptr<simpleTCP> proto);	
	    
            std::string rawcmd(const std::string& request);
            
            
        };
            
        
    }
    


}
