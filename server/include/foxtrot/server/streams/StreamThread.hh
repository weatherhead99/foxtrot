#pragma once
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <foxtrot/Logging.h>

using duration_ms = std::chrono::milliseconds;
using time_point = std::chrono::time_point<std::chrono::system_clock>;

namespace foxtrot
{
    class StreamThread
    {
    public:
        StreamThread(duration_ms update_tick, int id);
        
        void pause();
        void resume();
        
        std::thread& getthread();
        
    private:
        bool wait_until_next_update();
        
        duration_ms _update_tick;
        std::condition_variable _pause_resume_cv;
        std::mutex _pause_resume_mut;
        bool _paused = false;
        foxtrot::Logging _lg;
        
        time_point _started_time;
        
        std::thread _thrd;
        int _id;
    };
    
    
}
