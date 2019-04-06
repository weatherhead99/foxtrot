#pragma once
#include <string>
#include <mutex>
#include <future>

#include <boost/filesystem.hpp>

#include <foxtrot/Logging.h>

#include "autofill_common.hh"

using std::string;


namespace foxtrot {
 
    class autofill_logger {
    public:
        autofill_logger(const string& outfdir, int rotate_time_mins);
        ~autofill_logger();
        
        void LogEvent(const foxtrot::event_data& evdat);
        void LogEnvData(const foxtrot::env_data& dat);
        void LogComment(const string& comment);
        void start_new_logfile(const string& name);
        
        string get_date_time();
        
        std::future<std::exception_ptr> rotate_files_async_start(const string& basefname);
        
    private:
        int rotate_time_mins_;
        Logging _lg;
        std::string outfdir_;
        boost::filesystem::ofstream* _thisfile = nullptr;
        std::mutex file_swap_m;
    };
    
    
}
