#pragma once
#include "autofill_common.hh"
#include <string>
#include "Logging.h"
#include <boost/filesystem.hpp>

using std::string;


namespace foxtrot {
 
    class autofill_logger {
    public:
        autofill_logger(const string& outfdir, int rotate_time_hours);
        ~autofill_logger();
        
        void LogEvent(const foxtrot::event_data& evdat);
        void LogEnvData(const foxtrot::env_data& dat);
        
    private:
        void start_new_logfile(const string& name);
        Logging _lg;
        std::string outfdir_;
        boost::filesystem::ofstream* _thisfile = nullptr;
        
    };
    
    
}
