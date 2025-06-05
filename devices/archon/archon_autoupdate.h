#pragma once

#include "archon.h"

#include <chrono>
#include <future>

namespace foxtrot
{
    namespace devices
    {
        
        class archon_autoupdate : public archon_legacy
        {
            RTTR_ENABLE()
        public:
            archon_autoupdate(std::shared_ptr<simpleTCP>proto, int ms_update);
        
        private:
            int _ms_update;
            
            std::future<void> _update_future;
            
            void timer_process();
            
        };
        
        
    }
    
    
}
