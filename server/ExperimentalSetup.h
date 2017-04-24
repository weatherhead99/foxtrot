#pragma once
#include "server/DeviceHarness.h"
#include <string>
#include "Logging.h"

namespace foxtrot
{
    
    class ExperimentalSetup
    {
    public:
        ExperimentalSetup(const std::string& setupfile, DeviceHarness& harness);
        ~ExperimentalSetup();
        
    private:
       Logging _lg;
        void* _dl;
        DeviceHarness& _harness;
    };


}
