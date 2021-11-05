#include <foxtrot/server/streams/StreamThread.hh>

using namespace foxtrot;

StreamThread::StreamThread(duration_ms update_tick, int id) 
: _update_tick(update_tick), _lg("StreamThread"), _id(id)
{
    
}

bool StreamThread::wait_until_next_update()
{
    auto next_update = _started_time + _update_tick;
    std::unique_lock lck(_pause_resume_mut);
    //wait on the condition variable, if we time out we've hit the update, if 
    // the condition variable wait happens, we are now paused
    if( _pause_resume_cv.wait_until(lck, next_update, [this] {return _paused;}) )
    {
        _lg.strm(sl::trace) << "stream thread " << _id << " is paused..";
        return false;
    }
    else
    {
        _lg.strm(sl::trace) << "stream thread " << _id << "arrived at update";
        _started_time = std::chrono::system_clock::now();
        return true;
    }
}


std::thread& StreamThread::getthread()
{
    return _thrd;
}

void StreamThread::pause()
{
    if(!_paused)
    {
        std::lock_guard lck(_pause_resume_mut);
        _paused = true;
        _pause_resume_cv.notify_one();
    }
    
}

void StreamThread::resume()
{
    if(_paused)
    {
        std::lock_guard lck(_pause_resume_mut);
        _paused = false;
        _pause_resume_cv.notify_one();
    }
    
}
