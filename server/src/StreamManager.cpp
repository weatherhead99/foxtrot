#include <foxtrot/server/StreamManager.hh>
#include <foxtrot/ContentionError.h>

using namespace foxtrot;

rttr::variant Stream::execute(DeviceHarness& harness, const duration& timeout)
{
    auto dev = harness.GetDevice(devid);
    std::unique_lock lock(harness.GetMutex(devid), std::defer_lock);
    lock.try_lock_for(timeout);
    if(!lock.owns_lock())
        throw ContentionError("couldn't lock device with id: " + std::to_string(devid));
    
    return dev->Invoke(cap, capargs.begin(), capargs.end());
};
