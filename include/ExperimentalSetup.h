#pragma once
#include "server/DeviceHarness.h"
#include <string>

namespace foxtrot
{
    
    class ExperimentalSetup
    {
    public:
        ExperimentalSetup(const std::string& setupfile, DeviceHarness& harness);
        ~ExperimentalSetup();
        
    private:
        void* _dl;
        DeviceHarness& _harness;
    };


}
