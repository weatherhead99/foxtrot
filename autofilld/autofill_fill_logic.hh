#pragma once
#include "Logging.h"

namespace foxtrot {
    class Client;
    class autofill_logger;
    class Logging;
    
    class autofill_logic {
    public:
        autofill_logic(autofill_logger& logger);
        void fill_tank(Client& cl, int ws_devid, double filltime_hours, int relay);
        
        bool is_autofill_enabled(Client& cl);
        
        
    private:
        autofill_logger& logger_;
        bool fill_in_progress = false;
        Logging lg_;
    };
    
    
    namespace detail
    {
        
        void execute_fill(Client& cl, int ws_devid, double filltime_hours, int relay);
    
    }
}
