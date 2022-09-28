#pragma once



#include <foxtrot/server/Device.h>

namespace foxtrot
{
    
    template<typename T>
    class GlobalDeviceLock : public virtual T
    {
    public:
        GlobalDeviceLock() : T(nullptr) {};
        virtual std::optional<Lock> obtain_lock(const Capability& cap) override
        {
            std::optional<Lock> out;
            Lock lck(_mut, std::defer_lock);
            out = std::move(lck);
            return out;
        }
        
        virtual bool hasLockImplementation() const override
        {
            return true;
        }
        
    private:
        std::timed_mutex _mut;
    };
    
    
}
