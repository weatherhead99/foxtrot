#pragma once
#include "autofill_common.hh"
#include <string>

using std::string;

namespace foxtrot {
 
    class autofill_logger {
    public:
        autofill_logger(const string& outfdir, int rotate_time_hours);
        ~autofill_logger();
        
        void LogEvent(const foxtrot::event_types& evtp);
        void LogEnvData(const foxtrot::env_data& dat);
        
    private:
        
    };
    
    
}
