#include "archon_autoupdate.h"
#include <thread>


foxtrot::devices::archon_autoupdate::archon_autoupdate(std::shared_ptr<simpleTCP> proto, int ms_update)
: archon_legacy(proto), _ms_update(ms_update)
{
    
    auto embedded_call = [this] () {timer_process();};
    _update_future = std::async(std::launch::async,embedded_call);
}

void foxtrot::devices::archon_autoupdate::timer_process()
{
    //sleep until time is up
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(_ms_update));
        update_state();
    }
    
    
    
    
}

